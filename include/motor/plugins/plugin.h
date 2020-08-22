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

// cr_op is passed into the guest process to indicate the current operation
// happening so the process can manage its internal data if it needs.
typedef enum cr_op {
  CR_LOAD = 0,
  CR_STEP = 1,
  CR_UNLOAD = 2,
  CR_CLOSE = 3,
} cr_op;

typedef enum cr_failure {
  CR_NONE,     // No error
  CR_SEGFAULT, // SIGSEGV / EXCEPTION_ACCESS_VIOLATION
  CR_ILLEGAL,  // illegal instruction (SIGILL) / EXCEPTION_ILLEGAL_INSTRUCTION
  CR_ABORT,    // abort (SIGBRT)
  CR_MISALIGN, // bus error (SIGBUS) / EXCEPTION_DATATYPE_MISALIGNMENT
  CR_BOUNDS,   // EXCEPTION_ARRAY_BOUNDS_EXCEEDED
  CR_STACKOVERFLOW,     // EXCEPTION_STACK_OVERFLOW
  CR_STATE_INVALIDATED, // one or more global data section changed and does
                        // not safely match basically a failure of
                        // cr_plugin_validate_sections
  CR_BAD_IMAGE,       // The binary is not valid - compiler is still writing it
  CR_INITIAL_FAILURE, // Plugin version 1 crashed, cannot rollback
  CR_OTHER,           // Unknown or other signal,
  CR_USER = 0x100,
} cr_failure;

// public interface for the plugin context, this has some user facing
// variables that may be used to manage reload feedback.
// - userdata may be used by the user to pass information between reloads
// - version is the reload counter (after loading the first instance it will
//   be 1, not 0)
// - failure is the (platform specific) last error code for any crash that may
//   happen to cause a rollback reload used by the crash protection system
typedef struct cr_plugin {
  void* p;
  void* userdata;
  unsigned int version;
  enum cr_failure failure;
  unsigned int next_version;
  unsigned int last_working_version;
} cr_plugin;


#endif // MOTOR_PLUGIN_H
