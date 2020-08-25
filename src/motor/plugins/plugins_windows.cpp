extern "C" {
#include <stdio.h>

#include "motor/log.h"
#include "motor/plugins/plugin.h"

#include "private_api.h"
}

#include <string>

// clang-format off
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <dbghelp.h>
// clang-format on
#if defined(_MSC_VER)
#pragma comment(lib, "dbghelp.lib")
#endif
using so_handle = HMODULE;

#ifdef UNICODE
#define CR_WINDOWS_ConvertPath(_newpath, _path)                                \
    std::wstring _newpath(cr_utf8_to_wstring(_path))

static std::wstring cr_utf8_to_wstring(const std::string& str) {
    int     wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, 0, 0);
    wchar_t wpath_small[MAX_PATH];
    std::unique_ptr<wchar_t[]> wpath_big;
    wchar_t*                   wpath = wpath_small;
    if (wlen > _countof(wpath_small)) {
        wpath_big = std::unique_ptr<wchar_t[]>(new wchar_t[wlen]);
        wpath = wpath_big.get();
    }

    if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wpath, wlen) != wlen) {
        return L"";
    }

    return wpath;
}
#else
#define CR_WINDOWS_ConvertPath(_newpath, _path)                                \
    const std::string& _newpath = _path
#endif // UNICODE

static time_t cr_last_write_time(const std::string& path) {
    CR_WINDOWS_ConvertPath(_path, path);
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesEx(_path.c_str(), GetFileExInfoStandard, &fad)) {
        return -1;
    }

    if (fad.nFileSizeHigh == 0 && fad.nFileSizeLow == 0) { return -1; }

    LARGE_INTEGER time;
    time.HighPart = fad.ftLastWriteTime.dwHighDateTime;
    time.LowPart = fad.ftLastWriteTime.dwLowDateTime;

    return static_cast<time_t>(time.QuadPart / 10000000 - 11644473600LL);
}

static bool cr_exists(const std::string& path) {
    CR_WINDOWS_ConvertPath(_path, path);
    return GetFileAttributes(_path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

static bool cr_copy(const std::string& from, const std::string& to) {
    CR_WINDOWS_ConvertPath(_from, from);
    CR_WINDOWS_ConvertPath(_to, to);
    return CopyFile(_from.c_str(), _to.c_str(), FALSE) ? true : false;
}

static void cr_del(const std::string& path) {
    CR_WINDOWS_ConvertPath(_path, path);
    DeleteFile(_path.c_str());
}

// If using Microsoft Visual C/C++ compiler we need to do some workaround the
// fact that the compiled binary has a fullpath to the PDB hardcoded inside
// it. This causes a lot of headaches when trying compile while debugging as
// the referenced PDB will be locked by the debugger.
// To solve this problem, we patch the binary to rename the PDB to something
// we know will be unique to our in-flight instance, so when debugging it will
// lock this unique PDB and the compiler will be able to overwrite the
// original one.
#if defined(_MSC_VER)
#include <crtdbg.h>
#include <limits.h>
#include <stdio.h>
#include <tchar.h>

static std::string cr_replace_extension(const std::string& filepath,
                                        const std::string& ext) {
    std::string folder, filename, old_ext;
    cr_split_path(filepath, folder, filename, old_ext);
    return folder + filename + ext;
}

template<class T>
static T struct_cast(void* ptr, LONG offset = 0) {
    return reinterpret_cast<T>(reinterpret_cast<intptr_t>(ptr) + offset);
}

// RSDS Debug Information for PDB files
using DebugInfoSignature = DWORD;
#define CR_RSDS_SIGNATURE 'SDSR'
struct cr_rsds_hdr {
    DebugInfoSignature signature;
    GUID               guid;
    long               version;
    char               filename[1];
};

static bool cr_pe_debugdir_rva(PIMAGE_OPTIONAL_HEADER optionalHeader,
                               DWORD& debugDirRva, DWORD& debugDirSize) {
    if (optionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        auto optionalHeader64 =
            struct_cast<PIMAGE_OPTIONAL_HEADER64>(optionalHeader);
        debugDirRva =
            optionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG]
                .VirtualAddress;
        debugDirSize =
            optionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
    } else {
        auto optionalHeader32 =
            struct_cast<PIMAGE_OPTIONAL_HEADER32>(optionalHeader);
        debugDirRva =
            optionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG]
                .VirtualAddress;
        debugDirSize =
            optionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
    }

    if (debugDirRva == 0 && debugDirSize == 0) {
        return true;
    } else if (debugDirRva == 0 || debugDirSize == 0) {
        return false;
    }

    return true;
}

static bool cr_pe_fileoffset_rva(PIMAGE_NT_HEADERS ntHeaders, DWORD rva,
                                 DWORD& fileOffset) {
    bool  found = false;
    auto* sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections;
         i++, sectionHeader++) {
        auto sectionSize = sectionHeader->Misc.VirtualSize;
        if ((rva >= sectionHeader->VirtualAddress) &&
            (rva < sectionHeader->VirtualAddress + sectionSize)) {
            found = true;
            break;
        }
    }

    if (!found) { return false; }

    const int diff = static_cast<int>(sectionHeader->VirtualAddress -
                                      sectionHeader->PointerToRawData);
    fileOffset = rva - diff;
    return true;
}

static char* cr_pdb_find(LPBYTE imageBase, PIMAGE_DEBUG_DIRECTORY debugDir) {
    CR_ASSERT(debugDir && imageBase);
    LPBYTE     debugInfo = imageBase + debugDir->PointerToRawData;
    const auto debugInfoSize = debugDir->SizeOfData;
    if (debugInfo == 0 || debugInfoSize == 0) { return nullptr; }

    if (IsBadReadPtr(debugInfo, debugInfoSize)) { return nullptr; }

    if (debugInfoSize < sizeof(DebugInfoSignature)) { return nullptr; }

    if (debugDir->Type == IMAGE_DEBUG_TYPE_CODEVIEW) {
        auto signature = *(DWORD*) debugInfo;
        if (signature == CR_RSDS_SIGNATURE) {
            auto* info = (cr_rsds_hdr*) (debugInfo);
            if (IsBadReadPtr(debugInfo, sizeof(cr_rsds_hdr))) {
                return nullptr;
            }

            if (IsBadStringPtrA((const char*) info->filename, UINT_MAX)) {
                return nullptr;
            }

            return info->filename;
        }
    }

    return nullptr;
}

static bool cr_pdb_replace(const std::string& filename,
                           const std::string& pdbname, std::string& orig_pdb) {
    CR_WINDOWS_ConvertPath(_filename, filename);

    HANDLE fp = nullptr;
    HANDLE filemap = nullptr;
    LPVOID mem = 0;
    bool   result = false;
    do {
        fp = CreateFile(_filename.c_str(), GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL, nullptr);
        if ((fp == INVALID_HANDLE_VALUE) || (fp == nullptr)) { break; }

        filemap = CreateFileMapping(fp, nullptr, PAGE_READWRITE, 0, 0, nullptr);
        if (filemap == nullptr) { break; }

        mem = MapViewOfFile(filemap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (mem == nullptr) { break; }

        auto dosHeader = struct_cast<PIMAGE_DOS_HEADER>(mem);
        if (dosHeader == 0) { break; }

        if (IsBadReadPtr(dosHeader, sizeof(IMAGE_DOS_HEADER))) { break; }

        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) { break; }

        auto ntHeaders =
            struct_cast<PIMAGE_NT_HEADERS>(dosHeader, dosHeader->e_lfanew);
        if (ntHeaders == 0) { break; }

        if (IsBadReadPtr(ntHeaders, sizeof(ntHeaders->Signature))) { break; }

        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) { break; }

        if (IsBadReadPtr(&ntHeaders->FileHeader, sizeof(IMAGE_FILE_HEADER))) {
            break;
        }

        if (IsBadReadPtr(&ntHeaders->OptionalHeader,
                         ntHeaders->FileHeader.SizeOfOptionalHeader)) {
            break;
        }

        if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
            ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
            break;
        }

        auto sectionHeaders = IMAGE_FIRST_SECTION(ntHeaders);
        if (IsBadReadPtr(sectionHeaders,
                         ntHeaders->FileHeader.NumberOfSections *
                             sizeof(IMAGE_SECTION_HEADER))) {
            break;
        }

        DWORD debugDirRva = 0;
        DWORD debugDirSize = 0;
        if (!cr_pe_debugdir_rva(&ntHeaders->OptionalHeader, debugDirRva,
                                debugDirSize)) {
            break;
        }

        if (debugDirRva == 0 || debugDirSize == 0) { break; }

        DWORD debugDirOffset = 0;
        if (!cr_pe_fileoffset_rva(ntHeaders, debugDirRva, debugDirOffset)) {
            break;
        }

        auto debugDir =
            struct_cast<PIMAGE_DEBUG_DIRECTORY>(mem, debugDirOffset);
        if (debugDir == 0) { break; }

        if (IsBadReadPtr(debugDir, debugDirSize)) { break; }

        if (debugDirSize < sizeof(IMAGE_DEBUG_DIRECTORY)) { break; }

        int numEntries = debugDirSize / sizeof(IMAGE_DEBUG_DIRECTORY);
        if (numEntries == 0) { break; }

        for (int i = 1; i <= numEntries; i++, debugDir++) {
            char* pdb = cr_pdb_find((LPBYTE) mem, debugDir);
            if (pdb) {
                auto len = strlen(pdb);
                if (len >= strlen(pdbname.c_str())) {
                    orig_pdb = pdb;
                    memcpy_s(pdb, len, pdbname.c_str(), pdbname.length());
                    pdb[pdbname.length()] = 0;
                    result = true;
                }
            }
        }
    } while (0);

    if (mem != nullptr) { UnmapViewOfFile(mem); }

    if (filemap != nullptr) { CloseHandle(filemap); }

    if ((fp != nullptr) && (fp != INVALID_HANDLE_VALUE)) { CloseHandle(fp); }

    return result;
}

bool static cr_pdb_process(const std::string& desination) {
    std::string folder, fname, ext, orig_pdb;
    cr_split_path(desination, folder, fname, ext);
    bool result = cr_pdb_replace(desination, fname + ".pdb", orig_pdb);
    result &= cr_copy(orig_pdb, cr_replace_extension(desination, ".pdb"));
    return result;
}
#endif // _MSC_VER

static void cr_pe_section_save(cr_plugin& ctx, cr_plugin_section_type::e type,
                               int64_t vaddr, int64_t base,
                               IMAGE_SECTION_HEADER& shdr) {
    const auto   version = cr_plugin_section_version::current;
    auto         p = (cr_internal*) ctx.p;
    auto         data = &p->data[type][version];
    const size_t old_size = data->size;
    data->base = base;
    data->ptr = (char*) vaddr;
    data->size = shdr.SizeOfRawData;
    data->data = CR_REALLOC(data->data, shdr.SizeOfRawData);
    if (old_size < shdr.SizeOfRawData) {
        memset((char*) data->data + old_size, '\0',
               shdr.SizeOfRawData - old_size);
    }
}

static bool cr_plugin_validate_sections(cr_plugin& ctx, so_handle handle,
                                        const std::string& imagefile,
                                        bool               rollback) {
    (void) imagefile;
    CR_ASSERT(handle);
    auto p = (cr_internal*) ctx.p;
    auto ntHeaders = ImageNtHeader(handle);
    auto base = ntHeaders->OptionalHeader.ImageBase;
    auto sectionHeaders = (IMAGE_SECTION_HEADER*) (ntHeaders + 1);
    bool result = true;
    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i) {
        auto          sectionHeader = sectionHeaders[i];
        const int64_t size = sectionHeader.SizeOfRawData;
        if (!strcmp((const char*) sectionHeader.Name, ".state")) {
            if (ctx.version || rollback) {
                result &= cr_plugin_section_validate(
                    ctx, cr_plugin_section_type::state,
                    base + sectionHeader.VirtualAddress, base, size);
            }
            if (result) {
                auto sec = cr_plugin_section_type::state;
                cr_pe_section_save(ctx, sec,
                                   base + sectionHeader.VirtualAddress, base,
                                   sectionHeader);
            }
        } else if (!strcmp((const char*) sectionHeader.Name, ".bss")) {
            if (ctx.version || rollback) {
                result &= cr_plugin_section_validate(
                    ctx, cr_plugin_section_type::bss,
                    base + sectionHeader.VirtualAddress, base, size);
            }
            if (result) {
                auto sec = cr_plugin_section_type::bss;
                cr_pe_section_save(ctx, sec,
                                   base + sectionHeader.VirtualAddress, base,
                                   sectionHeader);
            }
        }
    }
    return result;
}

static void cr_so_unload(cr_plugin& ctx) {
    auto p = (cr_internal*) ctx.p;
    CR_ASSERT(p->handle);
    FreeLibrary((HMODULE) p->handle);
}

static so_handle cr_so_load(const std::string& filename) {
    CR_WINDOWS_ConvertPath(_filename, filename);
    auto new_dll = LoadLibrary(_filename.c_str());
    if (!new_dll) { CR_ERROR("Couldn't load plugin: %d\n", GetLastError()); }
    return new_dll;
}

static cr_plugin_main_func cr_so_symbol(so_handle handle) {
    CR_ASSERT(handle);
    auto new_main = (cr_plugin_main_func) GetProcAddress(handle, CR_MAIN_FUNC);
    if (!new_main) {
        CR_ERROR("Couldn't find plugin entry point: %d\n", GetLastError());
    }
    return new_main;
}

static void cr_plat_init() {}

static int cr_seh_filter(cr_plugin& ctx, unsigned long seh,
                         LPEXCEPTION_POINTERS ep) {

    if (ctx.version == 1) { return EXCEPTION_CONTINUE_SEARCH; }

    ctx.version = ctx.last_working_version;
    switch (seh) {
    case EXCEPTION_ACCESS_VIOLATION:
        ctx.failure = CR_SEGFAULT;
        sys_stack_trace((void*) ep);
        return EXCEPTION_EXECUTE_HANDLER;
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        ctx.failure = CR_ILLEGAL;
        sys_stack_trace((void*) ep);
        return EXCEPTION_EXECUTE_HANDLER;
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        ctx.failure = CR_MISALIGN;
        sys_stack_trace((void*) ep);
        return EXCEPTION_EXECUTE_HANDLER;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        ctx.failure = CR_BOUNDS;
        sys_stack_trace((void*) ep);
        return EXCEPTION_EXECUTE_HANDLER;
    case EXCEPTION_STACK_OVERFLOW:
        ctx.failure = CR_STACKOVERFLOW;
        sys_stack_trace((void*) ep);
        return EXCEPTION_EXECUTE_HANDLER;
    default:
        break;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

static int cr_plugin_main(cr_plugin& ctx, cr_op operation) {
    auto p = (cr_internal*) ctx.p;
    __try {
        if (p->main) { return p->main(&ctx, operation); }
    } __except (
        cr_seh_filter(ctx, GetExceptionCode(), GetExceptionInformation())) {
        return -1;
    }
    return -1;
}
