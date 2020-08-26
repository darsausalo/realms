#ifndef MOTOR_SYS_SYS_H
#define MOTOR_SYS_SYS_H

/* Crash handling */
void sys_dump_stack_trace(void* context);
void sys_register_crash_handler();

void sys_set_sym_search_path(const char* sym_search_path);
void sys_set_interrupt_handler(void (*interrupt_handler_func)());

/* File system support */
char* sys_get_bindir();
char* sys_get_userdir();

void sys_normalize_ospath(char* path);

str_array_t sys_list_files(const char* path, const char* extension);

#endif // MOTOR_SYS_SYS_H
