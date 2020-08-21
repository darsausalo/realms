#ifndef MOTOR_API_H
#define MOTOR_API_H

typedef struct {
  const char* (*GetText)(int i);
} hostApi_t;

#endif // MOTOR_API_H
