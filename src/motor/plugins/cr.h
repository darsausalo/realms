/*

MIT License

Copyright (c) 2017 Danny Angelo Carminati Grein

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef __CR_H__
#define __CR_H__

// Overridable macros
#ifndef CR_LOG
#ifdef CR_DEBUG
#include <stdio.h>
#define CR_LOG(...) fprintf(stdout, __VA_ARGS__)
#else
#define CR_LOG(...)
#endif
#endif

#ifndef CR_ERROR
#ifdef CR_DEBUG
#include <stdio.h>
#define CR_ERROR(...) fprintf(stderr, __VA_ARGS__)
#else
#define CR_ERROR(...)
#endif
#endif

#ifndef CR_TRACE
#ifdef CR_DEBUG
#include <stdio.h>
#define CR_TRACE fprintf(stdout, "CR_TRACE: %s\n", __FUNCTION__);
#else
#define CR_TRACE
#endif
#endif

#ifndef CR_MAIN_FUNC
#define CR_MAIN_FUNC "cr_main"
#endif

#ifndef CR_ASSERT
#include <assert.h>
#define CR_ASSERT(e) assert(e)
#endif

#ifndef CR_REALLOC
#include <stdlib.h>
#define CR_REALLOC(ptr, size) ::realloc(ptr, size)
#endif

#ifndef CR_FREE
#include <stdlib.h>
#define CR_FREE(ptr) ::free(ptr)
#endif

#ifndef CR_MALLOC
#include <stdlib.h>
#define CR_MALLOC(size) ::malloc(size)
#endif

#if defined(_MSC_VER)
// we should probably push and pop this
#pragma warning(                                                               \
    disable : 4003) // not enough actual parameters for macro 'identifier'
#endif


#include <algorithm>
#include <cstring> // memcpy
#include <string>

#if defined(CR_WINDOWS)
#define CR_PATH_SEPARATOR '\\'
#define CR_PATH_SEPARATOR_INVALID '/'
#else
#define CR_PATH_SEPARATOR '/'
#define CR_PATH_SEPARATOR_INVALID '\\'
#endif

static void cr_split_path(std::string path, std::string& parent_dir,
                          std::string& base_name, std::string& ext) {
  std::replace(path.begin(), path.end(), CR_PATH_SEPARATOR_INVALID,
               CR_PATH_SEPARATOR);
  auto sep_pos = path.rfind(CR_PATH_SEPARATOR);
  auto dot_pos = path.rfind('.');

  if (sep_pos == std::string::npos) {
    parent_dir = "";
    if (dot_pos == std::string::npos) {
      ext = "";
      base_name = path;
    } else {
      ext = path.substr(dot_pos);
      base_name = path.substr(0, dot_pos);
    }
  } else {
    parent_dir = path.substr(0, sep_pos + 1);
    if (dot_pos == std::string::npos || sep_pos > dot_pos) {
      ext = "";
      base_name = path.substr(sep_pos + 1);
    } else {
      ext = path.substr(dot_pos);
      base_name = path.substr(sep_pos + 1, dot_pos - sep_pos - 1);
    }
  }
}

static std::string cr_version_path(const std::string& basepath,
                                   unsigned version,
                                   const std::string& temppath) {
  std::string folder, fname, ext;
  cr_split_path(basepath, folder, fname, ext);
  std::string ver = std::to_string(version);
#if defined(_MSC_VER)
  // When patching PDB file path in library file we will drop path and leave only file name.
  // Length of path is extra space for version number. Trim file name only if version number
  // length exceeds pdb folder path length. This is not relevant on other platforms.
  if (ver.size() > folder.size()) {
    fname = fname.substr(0, fname.size() - (ver.size() - folder.size()));
  }
#endif
  if (!temppath.empty()) { folder = temppath; }
  return folder + fname + ver + ext;
}

namespace cr_plugin_section_type {
enum e { state, bss, count };
}

namespace cr_plugin_section_version {
enum e { backup, current, count };
}

struct cr_plugin_section {
  cr_plugin_section_type::e type = {};
  intptr_t base = 0;
  char* ptr = 0;
  int64_t size = 0;
  void* data = nullptr;
};

struct cr_plugin_segment {
  char* ptr = 0;
  int64_t size = 0;
};

typedef int (*cr_plugin_main_func)(struct cr_plugin* ctx, enum cr_op operation);

// keep track of some internal state about the plugin, should not be messed
// with by user
struct cr_internal {
  std::string fullname = {};
  std::string temppath = {};
  time_t timestamp = {};
  void* handle = nullptr;
  cr_plugin_main_func main = nullptr;
  cr_plugin_segment seg = {};
  cr_plugin_section data[cr_plugin_section_type::count]
                        [cr_plugin_section_version::count] = {};
};

static bool cr_plugin_section_validate(cr_plugin& ctx,
                                       cr_plugin_section_type::e type,
                                       intptr_t vaddr, intptr_t ptr,
                                       int64_t size);
static void cr_plugin_sections_reload(cr_plugin& ctx,
                                      cr_plugin_section_version::e version);
static void cr_plugin_sections_store(cr_plugin& ctx);
static void cr_plugin_sections_backup(cr_plugin& ctx);
static void cr_plugin_reload(cr_plugin& ctx);
static int cr_plugin_unload(cr_plugin& ctx, bool rollback, bool close);
static bool cr_plugin_changed(cr_plugin& ctx);
static bool cr_plugin_rollback(cr_plugin& ctx);
static int cr_plugin_main(cr_plugin& ctx, cr_op operation);

void cr_set_temporary_path(cr_plugin& ctx, const std::string& path) {
  auto pimpl = (cr_internal*) ctx.p;
  pimpl->temppath = path;
}

#if defined(CR_WINDOWS)

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
  int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, 0, 0);
  wchar_t wpath_small[MAX_PATH];
  std::unique_ptr<wchar_t[]> wpath_big;
  wchar_t* wpath = wpath_small;
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
  GUID guid;
  long version;
  char filename[1];
};

static bool cr_pe_debugdir_rva(PIMAGE_OPTIONAL_HEADER optionalHeader,
                               DWORD& debugDirRva, DWORD& debugDirSize) {
  if (optionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
    auto optionalHeader64 =
        struct_cast<PIMAGE_OPTIONAL_HEADER64>(optionalHeader);
    debugDirRva = optionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG]
                      .VirtualAddress;
    debugDirSize =
        optionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
  } else {
    auto optionalHeader32 =
        struct_cast<PIMAGE_OPTIONAL_HEADER32>(optionalHeader);
    debugDirRva = optionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG]
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
  bool found = false;
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
  LPBYTE debugInfo = imageBase + debugDir->PointerToRawData;
  const auto debugInfoSize = debugDir->SizeOfData;
  if (debugInfo == 0 || debugInfoSize == 0) { return nullptr; }

  if (IsBadReadPtr(debugInfo, debugInfoSize)) { return nullptr; }

  if (debugInfoSize < sizeof(DebugInfoSignature)) { return nullptr; }

  if (debugDir->Type == IMAGE_DEBUG_TYPE_CODEVIEW) {
    auto signature = *(DWORD*) debugInfo;
    if (signature == CR_RSDS_SIGNATURE) {
      auto* info = (cr_rsds_hdr*) (debugInfo);
      if (IsBadReadPtr(debugInfo, sizeof(cr_rsds_hdr))) { return nullptr; }

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
  bool result = false;
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
    if (IsBadReadPtr(sectionHeaders, ntHeaders->FileHeader.NumberOfSections *
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

    auto debugDir = struct_cast<PIMAGE_DEBUG_DIRECTORY>(mem, debugDirOffset);
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
  const auto version = cr_plugin_section_version::current;
  auto p = (cr_internal*) ctx.p;
  auto data = &p->data[type][version];
  const size_t old_size = data->size;
  data->base = base;
  data->ptr = (char*) vaddr;
  data->size = shdr.SizeOfRawData;
  data->data = CR_REALLOC(data->data, shdr.SizeOfRawData);
  if (old_size < shdr.SizeOfRawData) {
    memset((char*) data->data + old_size, '\0', shdr.SizeOfRawData - old_size);
  }
}

static bool cr_plugin_validate_sections(cr_plugin& ctx, so_handle handle,
                                        const std::string& imagefile,
                                        bool rollback) {
  (void) imagefile;
  CR_ASSERT(handle);
  auto p = (cr_internal*) ctx.p;
  auto ntHeaders = ImageNtHeader(handle);
  auto base = ntHeaders->OptionalHeader.ImageBase;
  auto sectionHeaders = (IMAGE_SECTION_HEADER*) (ntHeaders + 1);
  bool result = true;
  for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i) {
    auto sectionHeader = sectionHeaders[i];
    const int64_t size = sectionHeader.SizeOfRawData;
    if (!strcmp((const char*) sectionHeader.Name, ".state")) {
      if (ctx.version || rollback) {
        result &= cr_plugin_section_validate(
            ctx, cr_plugin_section_type::state,
            base + sectionHeader.VirtualAddress, base, size);
      }
      if (result) {
        auto sec = cr_plugin_section_type::state;
        cr_pe_section_save(ctx, sec, base + sectionHeader.VirtualAddress, base,
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
        cr_pe_section_save(ctx, sec, base + sectionHeader.VirtualAddress, base,
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
#ifndef __MINGW32__
  __try {
#endif
    if (p->main) { return p->main(&ctx, operation); }
#ifndef __MINGW32__
  } __except (
      cr_seh_filter(ctx, GetExceptionCode(), GetExceptionInformation())) {
    return -1;
  }
#endif
  return -1;
}

#endif // CR_WINDOWS

#if defined(CR_LINUX) || defined(CR_OSX)

#include <csignal>
#include <cstring>
#include <dlfcn.h>
#include <fcntl.h>
#include <setjmp.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ucontext.h>
#include <unistd.h>

#if defined(CR_LINUX)
#include <sys/sendfile.h> // sendfile
#elif defined(CR_OSX)
#include <copyfile.h> // copyfile
#endif

using so_handle = void*;

static time_t cr_last_write_time(const std::string& path) {
  struct stat stats;
  if (stat(path.c_str(), &stats) == -1) { return -1; }

  if (stats.st_size == 0) { return -1; }

#if defined(CR_OSX)
  return stats.st_mtime;
#else
  return stats.st_mtim.tv_sec;
#endif
}

static bool cr_exists(const std::string& path) {
  struct stat stats {};
  return stat(path.c_str(), &stats) != -1;
}

static bool cr_copy(const std::string& from, const std::string& to) {
#if defined(CR_LINUX)
  // Reference: http://www.informit.com/articles/article.aspx?p=23618&seqNum=13
  int input, output;
  struct stat src_stat;
  if ((input = open(from.c_str(), O_RDONLY)) == -1) { return false; }
  fstat(input, &src_stat);

  if ((output = open(to.c_str(), O_WRONLY | O_CREAT,
                     O_NOFOLLOW | src_stat.st_mode)) == -1) {
    close(input);
    return false;
  }

  int result = sendfile(output, input, NULL, src_stat.st_size);
  close(input);
  close(output);
  return result > -1;
#elif defined(CR_OSX)
  return copyfile(from.c_str(), to.c_str(), NULL,
                  COPYFILE_ALL | COPYFILE_NOFOLLOW_DST) == 0;
#endif
}

static void cr_del(const std::string& path) { unlink(path.c_str()); }

// unix,internal
// a helper function to validate that an area of memory is empty
// this is used to validate that the data in the .bss haven't changed
// and that we are safe to discard it and uses the new one.
bool cr_is_empty(const void* const buf, int64_t len) {
  if (!buf || !len) { return true; }

  bool r = false;
  auto c = (const char* const) buf;
  for (int i = 0; i < len; ++i) { r |= c[i]; }
  return !r;
}

#if defined(CR_LINUX)
#include <elf.h>
#include <link.h>

static size_t cr_file_size(const std::string& path) {
  struct stat stats;
  if (stat(path.c_str(), &stats) == -1) { return 0; }
  return static_cast<size_t>(stats.st_size);
}

// unix,internal
// save section information to be used during load/unload when copying
// around global state (from .bss and .state binary sections).
// vaddr = is the in memory loaded address of the segment-section
// base = is the in file section address
// shdr = the in file section header
template<class H>
void cr_elf_section_save(cr_plugin& ctx, cr_plugin_section_type::e type,
                         int64_t vaddr, int64_t base, H shdr) {
  const auto version = cr_plugin_section_version::current;
  auto p = (cr_internal*) ctx.p;
  auto data = &p->data[type][version];
  const size_t old_size = data->size;
  data->base = base;
  data->ptr = (char*) vaddr;
  data->size = shdr.sh_size;
  data->data = CR_REALLOC(data->data, shdr.sh_size);
  if (old_size < shdr.sh_size) {
    memset((char*) data->data + old_size, '\0', shdr.sh_size - old_size);
  }
}

// unix,internal
// validates that the sections being loaded are compatible with the previous
// one accordingly with desired `cr_mode` mode. If this is a first load, a
// validation is not necessary. At the same time it will initialize the
// section tracking information and alloc the required temporary space to use
// during unload.
template<class H>
bool cr_elf_validate_sections(cr_plugin& ctx, bool rollback, H shdr, int shnum,
                              const char* sh_strtab_p) {
  CR_ASSERT(sh_strtab_p);
  auto p = (cr_internal*) ctx.p;
  bool result = true;
  for (int i = 0; i < shnum; ++i) {
    const char* name = sh_strtab_p + shdr[i].sh_name;
    auto sectionHeader = shdr[i];
    const int64_t addr = sectionHeader.sh_addr;
    const int64_t size = sectionHeader.sh_size;
    const int64_t base = (intptr_t) p->seg.ptr + p->seg.size;
    if (!strcmp(name, ".state")) {
      const int64_t vaddr = base - size;
      auto sec = cr_plugin_section_type::state;
      if (ctx.version || rollback) {
        result &= cr_plugin_section_validate(ctx, sec, vaddr, addr, size);
      }
      if (result) { cr_elf_section_save(ctx, sec, vaddr, addr, sectionHeader); }
    } else if (!strcmp(name, ".bss")) {
      // .bss goes past segment filesz, but it may be just padding
      const int64_t vaddr = base;
      auto sec = cr_plugin_section_type::bss;
      if (ctx.version || rollback) {
        // this is kinda hack to skip bss validation if our data is zero
        // this means we don't care scrapping it, and helps skipping
        // validating a .bss that serves only as padding in the segment.
        if (!cr_is_empty(p->data[sec][0].data, p->data[sec][0].size)) {
          result &= cr_plugin_section_validate(ctx, sec, vaddr, addr, size);
        }
      }
      if (result) { cr_elf_section_save(ctx, sec, vaddr, addr, sectionHeader); }
    }
  }
  return result;
}

struct cr_ld_data {
  cr_plugin* ctx = nullptr;
  int64_t data_segment_address = 0;
  int64_t data_segment_size = 0;
  const char* fullname = nullptr;
};

// Iterate over all loaded shared objects and then for each one, iterates
// over each segment.
// So we find our plugin by filename and try to find the segment that
// contains our data sections (.state and .bss) to find their virtual
// addresses.
// We search segments with type PT_LOAD (1), meaning it is a loadable
// segment (anything that really matters ie. .text, .data, .bss, etc...)
// The segment where the p_memsz is bigger than p_filesz is the segment
// that contains the section .bss (if there is one or there is padding).
// Also, the segment will have sensible p_flags value (PF_W for exemple).
//
// Some useful references:
// http://www.skyfree.org/linux/references/ELF_Format.pdf
// https://eli.thegreenplace.net/2011/08/25/load-time-relocation-of-shared-libraries/
static int cr_dl_header_handler(struct dl_phdr_info* info, size_t, void* data) {
  CR_ASSERT(info && data);
  auto p = (cr_ld_data*) data;
  auto ctx = p->ctx;
  if (strcasecmp(info->dlpi_name, p->fullname)) { return 0; }

  for (int i = 0; i < info->dlpi_phnum; i++) {
    auto phdr = info->dlpi_phdr[i];
    if (phdr.p_type != PT_LOAD) { continue; }

    // assume the first writable segment is the one that contains our
    // sections this may not be true I imagine, but if this becomes an
    // issue we fix it by comparing against section addresses, but this
    // will require some rework on the code flow.
    if (phdr.p_flags & PF_W) {
      auto pimpl = (cr_internal*) ctx->p;
      pimpl->seg.ptr = (char*) (info->dlpi_addr + phdr.p_vaddr);
      pimpl->seg.size = phdr.p_filesz;
      break;
    }
  }
  return 0;
}

static bool cr_plugin_validate_sections(cr_plugin& ctx, so_handle handle,
                                        const std::string& imagefile,
                                        bool rollback) {
  CR_ASSERT(handle);
  cr_ld_data data;
  data.ctx = &ctx;
  auto pimpl = (cr_internal*) ctx.p;
  if (pimpl->mode == CR_DISABLE) { return true; }
  data.fullname = imagefile.c_str();
  dl_iterate_phdr(cr_dl_header_handler, (void*) &data);

  const auto len = cr_file_size(imagefile);
  char* p = nullptr;
  bool result = false;
  do {
    int fd = open(imagefile.c_str(), O_RDONLY);
    p = (char*) mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    // The ElfW() macro definition turns its argument into the name of an
    // ELF data type suitable for the hardware architecture. For example,
    // ElfW(Ehdr) yeilds the data type name Elf32_Ehdr on a 32-bit platforms,
    // and Elf64_Ehdr on 64-bit platforms.
    ElfW(Ehdr)* ehdr = (ElfW(Ehdr)*) p;
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
      break;
    }

    ElfW(Shdr*) shdr = (ElfW(Shdr)*) (p + ehdr->e_shoff);
    auto sh_strtab = &shdr[ehdr->e_shstrndx];
    const char* const sh_strtab_p = p + sh_strtab->sh_offset;
    result = cr_elf_validate_sections(ctx, rollback, shdr, ehdr->e_shnum,
                                      sh_strtab_p);
  } while (0);

  if (p) { munmap(p, len); }

  if (!result) { ctx.failure = CR_STATE_INVALIDATED; }

  return result;
}

#elif defined(CR_OSX)
#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#include <mach-o/ldsyms.h>
#include <stdlib.h> // realpath
#include <limits.h> // PATH_MAX

#if __LP64__
typedef struct mach_header_64 macho_hdr;
#define CR_MH_MAGIC MH_MAGIC_64
#else
typedef struct mach_header macho_hdr;
#define CR_MH_MAGIC MH_MAGIC
#endif

// osx,internal
// save section information to be used during load/unload when copying
// around global state (from .bss and .state binary sections).
// vaddr = is the in memory loaded address of the segment-section
void cr_macho_section_save(cr_plugin& ctx, cr_plugin_section_type::e type,
                           intptr_t addr, size_t size) {
  const auto version = cr_plugin_section_version::current;
  auto p = (cr_internal*) ctx.p;
  auto data = &p->data[type][version];
  const size_t old_size = data->size;
  data->base = 0;
  data->ptr = (char*) addr;
  data->size = size;
  data->data = CR_REALLOC(data->data, size);
  if (old_size < size) {
    memset((char*) data->data + old_size, '\0', size - old_size);
  }
}

// Iterate over all loaded shared objects and then for each one to find
// our plugin by filename. Then knowing its image index we can get our
// data sections (__state and __bss) and calculate their virtual
// addresses.
//
// Some useful references:
// man 3 dyld
static bool cr_plugin_validate_sections(cr_plugin& ctx, so_handle handle,
                                        const std::string& imagefile,
                                        bool rollback) {
  bool result = true;
  auto pimpl = (cr_internal*) ctx.p;
  if (pimpl->mode == CR_DISABLE) { return result; }
  CR_TRACE

  // resolve absolute path of the image, because _dyld_get_image_name returns abs path
  char imageAbsPath[PATH_MAX + 1];
  if (!::realpath(imagefile.c_str(), imageAbsPath)) {
    CR_ASSERT(0 && "resolving absolute path for plugin failed");
    return false;
  }

  const int count = (int) _dyld_image_count();
  for (int i = 0; i < count; i++) {
    const char* name = _dyld_get_image_name(i);

    if (strcasecmp(name, imageAbsPath)) {
      // match loaded image filename
      continue;
    }

    const auto hdr = _dyld_get_image_header(i);
    if (hdr->filetype != MH_DYLIB) {
      // assure it is a valid dylib
      continue;
    }

    intptr_t vaddr = _dyld_get_image_vmaddr_slide(i);
    (void) vaddr;
    //auto cmd_stride = sizeof(struct mach_header);
    if (hdr->magic != CR_MH_MAGIC) {
      // check for conforming mach-o header
      continue;
    }

    auto validate_and_save = [&](cr_plugin_section_type::e sec, intptr_t addr,
                                 unsigned long size) {
      if (addr != 0 && size != 0) {
        if (ctx.version || rollback) {
          result &= cr_plugin_section_validate(ctx, sec, addr, 0, size);
        }
        if (result) { cr_macho_section_save(ctx, sec, addr, size); }
      }
    };

    auto mhdr = (macho_hdr*) hdr;
    unsigned long size = 0;
    auto ptr = (intptr_t) getsectiondata(mhdr, SEG_DATA, "__bss", &size);
    validate_and_save(cr_plugin_section_type::bss, ptr, (size_t) size);
    if (result) {
      ptr = (intptr_t) getsectiondata(mhdr, SEG_DATA, "__state", &size);
      validate_and_save(cr_plugin_section_type::state, ptr, (size_t) size);
    }
    break;
  }

  return result;
}

#endif

static void cr_so_unload(cr_plugin& ctx) {
  CR_ASSERT(ctx.p);
  auto p = (cr_internal*) ctx.p;
  CR_ASSERT(p->handle);

  const int r = dlclose(p->handle);
  if (r) { CR_ERROR("Error closing plugin: %d\n", r); }

  p->handle = nullptr;
  p->main = nullptr;
}

static so_handle cr_so_load(const std::string& new_file) {
  dlerror();
  auto new_dll = dlopen(new_file.c_str(), RTLD_NOW);
  if (!new_dll) { CR_ERROR("Couldn't load plugin: %s\n", dlerror()); }
  return new_dll;
}

static cr_plugin_main_func cr_so_symbol(so_handle handle) {
  CR_ASSERT(handle);
  dlerror();
  auto new_main = (cr_plugin_main_func) dlsym(handle, CR_MAIN_FUNC);
  if (!new_main) {
    CR_ERROR("Couldn't find plugin entry point: %s\n", dlerror());
  }
  return new_main;
}

sigjmp_buf env;

static void cr_signal_handler(int sig, siginfo_t* si, void* uap) {
  CR_TRACE(void) uap;
  CR_ASSERT(si);
  siglongjmp(env, sig);
}

static void cr_plat_init() {
  CR_TRACE
  static bool initialized = false;
  if (initialized) { return; }
  initialized = true;
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO | SA_RESTART | SA_NODEFER;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = cr_signal_handler;
#if defined(CR_LINUX)
  sa.sa_restorer = nullptr;
#endif

  if (sigaction(SIGILL, &sa, nullptr) == -1) {
    CR_ERROR("Failed to setup SIGILL handler\n");
  }
  if (sigaction(SIGBUS, &sa, nullptr) == -1) {
    CR_ERROR("Failed to setup SIGBUS handler\n");
  }
  if (sigaction(SIGSEGV, &sa, nullptr) == -1) {
    CR_ERROR("Failed to setup SIGSEGV handler\n");
  }
  if (sigaction(SIGABRT, &sa, nullptr) == -1) {
    CR_ERROR("Failed to setup SIGABRT handler\n");
  }
}

static cr_failure cr_signal_to_failure(int sig) {
  switch (sig) {
  case 0:
    return CR_NONE;
  case SIGILL:
    return CR_ILLEGAL;
  case SIGBUS:
    return CR_MISALIGN;
  case SIGSEGV:
    return CR_SEGFAULT;
  case SIGABRT:
    return CR_ABORT;
  }
  return static_cast<cr_failure>(CR_OTHER + sig);
}

static int cr_plugin_main(cr_plugin& ctx, cr_op operation) {
  if (int sig = sigsetjmp(env, 1)) {
    ctx.version = ctx.last_working_version;
    ctx.failure = cr_signal_to_failure(sig);
    CR_LOG("1 FAILURE: %d (CR: %d)\n", sig, ctx.failure);
    return -1;
  } else {
    auto p = (cr_internal*) ctx.p;
    CR_ASSERT(p);
    if (p->main) { return p->main(&ctx, operation); }
  }

  return -1;
}

#endif // CR_LINUX || CR_OSX

#endif // __CR_H__                                                             \
       // clang-format off
/*
```

</details>
*/
