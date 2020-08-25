#include "private_api.h"

static struct {
    char* bindir;
    char* basedir;
    char* userdir;
} fs;

void fs_init(myml_table_t* options) {
    log_info("init fs");

    fs.bindir = sys_get_bindir();
    fs.userdir = sys_get_userdir();

    fs.basedir = mt_strdup(fs.bindir);

    fs.basedir = fs_append_path(fs.basedir, MOTOR_BASE_DIR);
    fs.userdir = fs_append_path(fs.userdir, MOTOR_USER_DIR);

    const char* basedir = myml_find_string(options, "fs.basedir");
    const char* userdir = myml_find_string(options, "fs.userdir");

    if (basedir) fs_replace_path(fs.basedir, basedir);
    if (userdir) fs_replace_path(fs.userdir, userdir);

    sys_normalize_ospath(fs.bindir);
    sys_normalize_ospath(fs.basedir);
    sys_normalize_ospath(fs.userdir);

    log_info("  bindir: %s", fs.bindir);
    log_info("  basedir: %s", fs.basedir);
    log_info("  userdir: %s", fs.userdir);
}

void fs_shutdown() {
    log_info("shutdown fs");

    mt_free(fs.bindir);
    mt_free(fs.basedir);
    mt_free(fs.userdir);
}

char* fs_append_path(char* path1, const char* path2) {
    mt_assert(path1, MT_INVALID_PARAMETER, NULL);

    mt_size_t len1 = mt_strlen(path1);
    mt_size_t len2 = mt_strlen(path2);

    path1 = mt_realloc(path1, len1 + len2 + 2);
    mt_strcpy(&path1[len1 + 1], path2);
    path1[len1] = '/';
    path1[len1 + len2 + 1] = '\0';
    return path1;
}

char* fs_replace_path(char* old_path, const char* new_path) {
    mt_assert(old_path, MT_INVALID_PARAMETER, NULL);
    old_path = mt_realloc(old_path, strlen(new_path) + 1);
    mt_strcpy(old_path, new_path);
    return old_path;
}

const char* fs_get_bindir_ospath() { return fs.bindir; }
const char* fs_get_basedir_ospath() { return fs.basedir; }
const char* fs_get_userdir_ospath() { return fs.userdir; }
