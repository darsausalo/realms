#ifndef MOTOR_SYS_SYS_H
#define MOTOR_SYS_SYS_H

void sys_stack_trace(void* context);
void sys_register_crash_handler(const char* sym_search_path);

#endif // MOTOR_SYS_SYS_H
