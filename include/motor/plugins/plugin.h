#ifndef MOTOR_PLUGIN_H
#define MOTOR_PLUGIN_H

#if defined(_MSC_VER)
#if defined(__cplusplus)
#define CR_EXPORT extern "C" __declspec(dllexport)
#else
#define CR_EXPORT __declspec(dllexport)
#endif
#endif // defined(_MSC_VER)

#if defined(__GNUC__) // clang & gcc
#if defined(__cplusplus)
#define CR_EXPORT extern "C" __attribute__((visibility("default")))
#else
#define CR_EXPORT __attribute__((visibility("default")))
#endif
#endif // defined(__GNUC__)

typedef enum {
    kPluginOpLoad = 0,
    kPluginOpStep = 1,
    kPluginOpUnload = 2,
    kPluginOpClose = 3,
} plugin_op_t;

typedef struct {
    uint32_t version;

    void* user_data; // TODO: world or api
    void* context;   // private context
} plugin_t;

#endif // MOTOR_PLUGIN_H
