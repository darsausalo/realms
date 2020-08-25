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
#include <limits.h> // PATH_MAX
#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#include <mach-o/ldsyms.h>
#include <stdlib.h> // realpath

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
