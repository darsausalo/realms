#ifndef MOTOR_SYS_SYS_H
#define MOTOR_SYS_SYS_H

/* Lifetime */
void sys_init();
void sys_shutdown();

/* Crash handling */
void sys_dump_stack_trace(void* context);
void sys_register_crash_handler();

void sys_set_sym_search_path(const char* sym_search_path);
void sys_set_interrupt_handler(void (*interrupt_handler_func)());

/* File system support */
FILE* sys_open_log_file();

sds sys_get_bindir();
sds sys_get_userdir();

str_array_t sys_list_files(const sds path, const sds extension);

#endif // MOTOR_SYS_SYS_H
