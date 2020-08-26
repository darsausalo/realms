#include "private_api.h"

typedef struct search_path_s {
    struct search_path_s* next;

    char* path;
} search_path_t;

static struct {
    char* bindir;
    char* basedir;
    char* userdir;

    search_path_t* search_paths;
} fs = {.bindir = NULL, .basedir = NULL, .userdir = NULL, .search_paths = NULL};

static void fs_add_search_path(const char* path) {
    search_path_t* search_path = mt_malloc(sizeof(search_path_t));
    search_path->path = mt_strdup(path);
    search_path->next = fs.search_paths;
    fs.search_paths = search_path;
}

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

    char* path = mt_strdup(fs.userdir);
    path = fs_append_path(path, "test1.txt");

    FILE* fp = fopen(path, "rt");
    if (fp) {
        log_warning("exists");
    } else {
        log_error("NOT exists");
    }
    log_info("тест");

    fs_add_search_path(fs.basedir);
    // fs_add_search_path(fs.userdir);

    log_info("  bindir: %s", fs.bindir);
    log_info("  basedir: %s", fs.basedir);
    log_info("  userdir: %s", fs.userdir);
}

void fs_shutdown() {
    log_info("shutdown fs");

    search_path_t* next;
    for (search_path_t* search_path = fs.search_paths; search_path;
         search_path = next) {
        next = search_path->next;
        mt_free(search_path->path);
        mt_free(search_path);
    }

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

static void fs_add_file_to_list(str_array_t files, char* filename) {
    for (int i = 0; i < darr_count(files); i++) {
        if (mt_strcasecmp(files[i], filename) != 0) { return; }
    }

    darr_push(files, mt_strdup(filename));
}

str_array_t fs_list_files(const char* directory, const char* extension) {
    str_array_t files = NULL;
    for (search_path_t* search_path = fs.search_paths; search_path;
         search_path = search_path->next) {
        char* path = fs_append_path(search_path->path, directory);
        sys_normalize_ospath(path);
        str_array_t sys_files = sys_list_files(path, extension);
        for (int i = 0; i < darr_count(sys_files); i++) {
            fs_add_file_to_list(files, sys_files[i]);
        }
        darr_free(sys_files);
        mt_free(path);
    }
    return files;
}

void fs_free_file_list(str_array_t list) {
    for (int i = 0; i < darr_count(list); i++) { mt_free(list[i]); }
    darr_free(list);
}
