#include "sys_windows.h"
#include <shlobj.h>

char* sys_get_bindir() {
    char* path = mt_malloc(256);

    HANDLE hInstance = GetModuleHandle(NULL);
    GetModuleFileName(hInstance, path, 256);

    int len = strlen(path);
    if (len > 1) {
        for (int i = len - 1; i > 0; i--) {
            if (path[i] == '\\') {
                path[i] = '\0';
                break;
            }
        }
    }

    return path;
}

char* sys_get_userdir() {
    char* path;
    char  folder[MAX_PATH];
    if (SHGetSpecialFolderPath(NULL, folder, CSIDL_PERSONAL, 1)) {
        path = mt_malloc(MAX_PATH);
        mt_strcpy(path, folder);
    } else {
        path = sys_get_bindir();
    }

    path = fs_append_path(path, MOTOR_PROJECT_ORG);
    path = fs_append_path(path, MOTOR_PROJECT_NAME);

    sys_normalize_ospath(path);

    return path;
}

void sys_normalize_ospath(char* path) {
    for (char* ch = &path[0]; *ch != '\0'; ch++) {
        if (*ch == '/') *ch = '\\';
    }
}
