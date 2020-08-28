#ifndef MOTOR_FS_H
#define MOTOR_FS_H

typedef int file_handle_t;

void fs_init(myml_table_t* options);
void fs_shutdown();

str_array_t fs_list_files(const sds directory, const sds extension);
void        fs_free_file_list(str_array_t list);

#endif // MOTOR_FS_H
