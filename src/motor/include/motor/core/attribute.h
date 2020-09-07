#ifndef MOTOR_ATTRIBUTE_H
#define MOTOR_ATTRIBUTE_H

#ifndef MOTOR_EXPORT
#if defined _WIN32 || _MSC_VER
#define MOTOR_EXPORT extern "C" __declspec(dllexport)
#define MOTOR_IMPORT extern "C" __declspec(dllimport)
#elif defined __GNUC__ && __GNUC__ >= 4
#else
#define MOTOR_EXPORT extern "C" __attribute__((visibility("default")))
#define MOTOR_IMPORT extern "C" __attribute__((visibility("default")))
#endif
#endif

#endif // MOTOR_ATTRIBUTE_H
