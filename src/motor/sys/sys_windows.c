#include "sys_windows.h"
#include <shlobj.h>
#include <direct.h>
#include <io.h>

static HANDLE   sys_hInstance;
static wchar_t* sys_bindir;
static wchar_t* sys_userdir;
static FILE*    sys_log_fp;

#define _mt_mb_to_wchar(s, ws)                                                 \
    wchar_t*##ws;                                                              \
    {                                                                          \
        int wlen = MultiByteToWideChar(CP_UTF8, 0, s, -1, 0, 0);               \
        ws = mt_alloca(wlen * mt_sizeof(wchar_t));                             \
        if (MultiByteToWideChar(CP_UTF8, 0, s, -1, ws, wlen) != wlen) {        \
            ws = NULL;                                                         \
        }                                                                      \
    }

static wchar_t* sys_mb_to_wchar(const sds s) {
    int      wlen = MultiByteToWideChar(CP_UTF8, 0, s, -1, 0, 0);
    wchar_t* wpath = mt_malloc(wlen * mt_sizeof(wchar_t));

    if (MultiByteToWideChar(CP_UTF8, 0, s, -1, wpath, wlen) != wlen) {
        wpath[0] = L'\0';
    }

    return wpath;
}

static sds sys_wchar_to_mb(const wchar_t* s) {
    int   len = WideCharToMultiByte(CP_UTF8, 0, s, -1, 0, 0, NULL, NULL);
    char* path = mt_alloca(len);

    if (WideCharToMultiByte(CP_UTF8, 0, s, -1, path, len, NULL, NULL) != len) {
        sdssetlen(path, 0);
    }

    return sdsnew(path);
}

static void sys_normalize_ospath(wchar_t* path) {
    for (wchar_t* ch = &path[0]; *ch != '\0'; ch++) {
        if (*ch == '/') *ch = '\\';
    }
}

static void sys_init_bindir() {
    sys_bindir = malloc(MAX_PATH * sizeof(wchar_t) + 1);
    DWORD len = GetModuleFileNameW(sys_hInstance, sys_bindir, MAX_PATH - 1);

    for (size_t i = len - 1; i >= 0; i--) {
        if (sys_bindir[i] == '\\') {
            sys_bindir[i] = '\0';
            break;
        }
    }
}

static void sys_init_userdir() {
    wchar_t* path;
    wchar_t  folder[MAX_PATH];
    if (SHGetSpecialFolderPathW(NULL, folder, CSIDL_PERSONAL, TRUE)) {
        path = folder;
    } else {
        path = sys_bindir;
    }

    size_t   project_org_len = strlen(MOTOR_PROJECT_ORG);
    wchar_t* project_org = mt_alloca((project_org_len + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, MOTOR_PROJECT_ORG, -1, project_org,
                        project_org_len + 1);

    size_t   project_name_len = strlen(MOTOR_PROJECT_NAME);
    wchar_t* project_name = mt_alloca((project_name_len + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, MOTOR_PROJECT_NAME, -1, project_name,
                        project_name_len + 1);

    size_t path_len = wcslen(path);
    size_t len = path_len + 1 + project_org_len + 1 + project_name_len;

    sys_userdir = malloc((len + 1) * sizeof(wchar_t));
    wcscpy(sys_userdir, path);
    sys_userdir[path_len] = '\\';
    wcscpy(&sys_userdir[path_len + 1], project_org);
    sys_userdir[path_len + 1 + project_org_len] = '\\';
    wcscpy(&sys_userdir[path_len + 1 + project_org_len + 1], project_name);
    sys_userdir[len] = '\0';
}

void sys_init() {
    sys_hInstance = GetModuleHandle(NULL);
    SetConsoleOutputCP(CP_UTF8);

    sys_register_crash_handler();

    sys_init_bindir();
    sys_init_userdir();
}

void sys_shutdown() {
    free(sys_bindir);
    free(sys_userdir);
}

FILE* sys_open_log_file() {
    size_t   log_filename_len = strlen(MOTOR_LOG_FILENAME);
    wchar_t* log_filename = mt_alloca((log_filename_len + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, MOTOR_LOG_FILENAME, -1, log_filename,
                        log_filename_len + 1);

    size_t   bin_path_len = wcslen(sys_bindir);
    size_t   log_path_len = bin_path_len + 1 + log_filename_len;
    wchar_t* log_path = mt_alloca((log_path_len + 1) * sizeof(wchar_t));

    wcscpy(log_path, sys_bindir);
    log_path[bin_path_len] = '\\';
    wcscpy(&log_path[bin_path_len + 1], log_filename);
    log_path[log_path_len] = '\0';

    return _wfopen(log_path, L"wt");
}

sds sys_get_bindir() { return sys_wchar_to_mb(sys_bindir); }

sds sys_get_userdir() { return sys_wchar_to_mb(sys_userdir); }

str_array_t sys_list_files(const sds path, const sds extension) {
    // struct _finddata_t findinfo;
    // str_array_t        files = NULL;
    // wchar_t            search[MAX_PATH];

    // MultiByteToWideChar(CP_UTF8, 0, path, -1, search, MAX_PATH);
    // wsprintf()

    // intptr_t file_handle = _findfirst(ospath, &findinfo);

    // return files;
    return NULL;
}
