#ifndef MOTOR_PRIVATE_PLUGIN_H
#define MOTOR_PRIVATE_PLUGIN_H

// TODO: replace by log_*
#define CR_TRACE log_trace("%s", __FUNCTION__);
#define CR_LOG(...) log_debug(__VA_ARGS__)
#define CR_ERROR(...) log_error(__VA_ARGS__)

//
// Global OS specific defines/customizations
//
#if defined(_WIN32)
#define CR_WINDOWS
#define CR_PLUGIN(name) "" name ".dll"
#elif defined(__linux__)
#define CR_LINUX
#define CR_PLUGIN(name) "lib" name ".so"
#elif defined(__APPLE__)
#define CR_OSX
#define CR_PLUGIN(name) "lib" name ".dylib"
#else
#error "Unknown/unsupported platform, please open an issue if you think this \
platform should be supported."
#endif // CR_WINDOWS || CR_LINUX || CR_OSX

void phost_init(const char* bin_dir);
void phost_shutdown();

plugin_t* phost_open_plugin(const char* name);
void      phost_close_plugin(plugin_t* plugin);

bool phost_load_plugin(plugin_t* plugin);
int  phost_update_plugin(plugin_t* plugin);


//-----------------------------------------------------------------------------
// Internal
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Defs

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

#if defined(CR_WINDOWS)
#define CR_PATH_SEPARATOR '\\'
#define CR_PATH_SEPARATOR_INVALID '/'
#else
#define CR_PATH_SEPARATOR '/'
#define CR_PATH_SEPARATOR_INVALID '\\'
#endif

//-----------------------------------------------------------------------------
// Types

typedef enum {
    kPluginSectionTypeState = 0,
    kPluginSectionTypeBSS = 1,
    kPluginSectionTypeMax = 2,
} plugin_section_type_t;

typedef enum {
    kPluginSectionVersionCurrent = 0,
    kPluginSectionVersionBackup = 1,
    kPluginSectionVersionMax = 2,
} plugin_section_version_t;

typedef struct {
    plugin_section_type_t type;
    intptr_t              base;
    char*                 ptr;
    int64_t               size;
    void*                 data;
} plugin_section_t;

typedef struct {
    char*   ptr;
    int64_t size;
} plugin_segment_t;

typedef int (*plugin_entry_func)(plugin_t* plugin, plugin_op_t op);

typedef enum {
    kPluginErrorNone = 0,
    kPluginErrorSegfault,
    kPluginErrorIllegal,
    kPluginErrorAbort,
    kPluginErrorMisalign,
    kPluginErrorBounds,
    kPluginErrorStackoverflow,
    kPluginErrorState_Invalidated,
    kPluginErrorBad_Image,
    kPluginErrorInitial_Failure,
    kPluginErrorOther,
    kPluginErrorUser = 0x100,
} plugin_error_t;

// keep track of some internal state about the plugin, should not be messed
// with by user
struct {
    const char* name;

    char*  path;
    time_t timestamp;

    plugin_error_t last_error;

    uint32_t next_version;
    uint32_t last_working_version;

    void*             handle;
    plugin_entry_func entry;

    plugin_segment_t seg;
    plugin_section_t data[kPluginSectionTypeMax][kPluginSectionVersionMax];
} plugin_context_t;

//-----------------------------------------------------------------------------
// Funcs

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
                                   unsigned           version) {
    std::string folder, fname, ext;
    cr_split_path(basepath, folder, fname, ext);
    std::string ver = std::to_string(version);
    char        ver[16];
    sprintf(ver, "%d", version);
#if defined(_MSC_VER)
    // When patching PDB file path in library file we will drop path and leave only file name.
    // Length of path is extra space for version number. Trim file name only if version number
    // length exceeds pdb folder path length. This is not relevant on other platforms.
    if (ver.size() > folder.size()) {
        fname = fname.substr(0, fname.size() - (ver.size() - folder.size()));
    }
#endif
    return folder + fname + ver + ext;
}

bool cr_plugin_section_validate(cr_plugin& ctx, cr_plugin_section_type_t type,
                                intptr_t vaddr, intptr_t ptr, int64_t size);
int  cr_plugin_unload(cr_plugin& ctx, bool rollback, bool close);
bool cr_plugin_changed(cr_plugin& ctx);
bool cr_plugin_rollback(cr_plugin& ctx);
int  cr_plugin_main(cr_plugin& ctx, cr_op operation);

#endif // MOTOR_PRIVATE_PLUGIN_H
