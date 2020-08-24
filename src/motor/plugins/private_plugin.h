#ifndef MOTOR_PRIVATE_PLUGIN_H
#define MOTOR_PRIVATE_PLUGIN_H

// TODO: replace by log_*
#define CR_TRACE log_trace("%s", __FUNCTION__);
#define CR_LOG(...) log_debug(__VA_ARGS__)
#define CR_ERROR(...) log_error(__VA_ARGS__)

//
// Global OS specific defines/customizations
//
#if defined(_WIN32)
#define CR_WINDOWS
#define CR_PLUGIN(name) "" name ".dll"
#elif defined(__linux__)
#define CR_LINUX
#define CR_PLUGIN(name) "lib" name ".so"
#elif defined(__APPLE__)
#define CR_OSX
#define CR_PLUGIN(name) "lib" name ".dylib"
#else
#error "Unknown/unsupported platform, please open an issue if you think this \
platform should be supported."
#endif // CR_WINDOWS || CR_LINUX || CR_OSX

// cr_mode defines how much we validate global state transfer between
// instances. The default is CR_UNSAFE, you can choose another mode by
// defining CR_HOST, ie.: #define CR_HOST CR_SAFEST
typedef enum cr_mode {
  CR_SAFEST = 0, // validate address and size of the state section, if
                 // anything changes the load will rollback
  CR_SAFE = 1,   // validate only the size of the state section, this means
                 // that address is assumed to be safe if avoided keeping
                 // references to global/static states
  CR_UNSAFE = 2, // don't validate anything but that the size of the section
                 // fits, may not be identical though
  CR_DISABLE = 3 // completely disable the auto state transfer
} cr_mode;

bool cr_plugin_open(cr_plugin* ctx, const char* fullpath);
int cr_plugin_update(cr_plugin* ctx, bool reloadCheck);
void cr_plugin_close(cr_plugin* ctx);

#endif // MOTOR_PRIVATE_PLUGIN_H
