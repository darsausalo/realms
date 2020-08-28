#include "private_api.h"

typedef struct search_path_s {
    struct search_path_s* next;

    sds path;
} search_path_t;

static struct {
    sds bindir;
    sds basedir;
    sds userdir;

    search_path_t* search_paths;
} fs = {.bindir = NULL, .basedir = NULL, .userdir = NULL, .search_paths = NULL};

static void fs_add_search_path(const sds path) {
    search_path_t* search_path = mt_malloc(sizeof(search_path_t));
    search_path->path = mt_strdup(path);
    search_path->next = fs.search_paths;
    fs.search_paths = search_path;
}

void fs_init(myml_table_t* options) {
    log_info("init fs");

    fs.bindir = sys_get_bindir();
    fs.userdir = sys_get_userdir();

    fprintf(stderr, "bindir: %s\n", fs.bindir);

    fs.basedir = sdsdup(fs.bindir);
    fprintf(stderr, "basedir: %s\n", fs.basedir);
    fprintf(stderr, "userdir: %s\n", fs.userdir);

    fs.basedir = sdscatfmt(fs.basedir, "/%s" MOTOR_BASE_DIR);
    fs.userdir = sdscatfmt(fs.userdir, "/%s" MOTOR_USER_DIR);

    const char* basedir = myml_find_string(options, "fs.basedir");
    const char* userdir = myml_find_string(options, "fs.userdir");

    if (basedir) fs.basedir = sdscpy(fs.basedir, basedir);
    if (userdir) fs.userdir = sdscpy(fs.userdir, userdir);

    fs_add_search_path(fs.basedir);
    fs_add_search_path(fs.userdir);

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

static void fs_add_file_to_list(str_array_t files, sds filename) {
    for (int i = 0; i < darr_count(files); i++) {
        if (mt_strcasecmp(files[i], filename) != 0) { return; }
    }

    darr_push(files, mt_strdup(filename));
}

str_array_t fs_list_files(const sds directory, const sds extension) {
    str_array_t files = NULL;
    for (search_path_t* search_path = fs.search_paths; search_path;
         search_path = search_path->next) {
        sds path = sdsdup(search_path->path);
        path = sdscatfmt(path, "/%S", directory);

        str_array_t sys_files = sys_list_files(path, extension);

        for (int i = 0; i < darr_count(sys_files); i++) {
            // TODO: check unique?
            fs_add_file_to_list(files, sys_files[i]);
        }

        darr_free(sys_files);
        sdsfree(path);
    }
    return files;
}

void fs_free_file_list(str_array_t list) {
    for (int i = 0; i < darr_count(list); i++) { mt_free(list[i]); }
    darr_free(list);
}
