#ifndef MOTOR_HOST_H
#define MOTOR_HOST_H

void host_init(int argc, char** argv);
void host_shutdown();

bool host_is_run();
void host_quit();
void host_abort();

#endif // MOTOR_HOST_H
