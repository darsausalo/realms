#ifndef MOTOR_FS_H
#define MOTOR_FS_H

typedef struct {
    const char* canonical;
    const char* os;
} fs_path_t;

typedef int file_handle_t;

void fs_init(myml_table_t* options);
void fs_shutdown();

char* fs_append_path(char* path1, const char* path2);
char* fs_replace_path(char* old_path, const char* new_path);

const char* fs_get_bindir();
const char* fs_get_basedir();
const char* fs_get_userdir();

str_array_t fs_list_files(const char* directory, const char* extension);
void        fs_free_file_list(str_array_t list);

#endif // MOTOR_FS_H
