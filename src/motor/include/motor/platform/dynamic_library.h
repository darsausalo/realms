#ifndef MOTOR_DYNAMIC_LIBRARY_H
#define MOTOR_DYNAMIC_LIBRARY_H

#if defined _WIN32
#define MOTOR_PLUGIN_NAME(name) "" name ".dll"
#elif defined __linux__
#define MOTOR_PLUGIN_NAME(name) "lib" name ".so"
#elif defined __APPLE__
#define MOTOR_PLUGIN_NAME(name) "lib" name ".dylib"
#else
#error "Unupported platofrm"
#endif

#include "motor/host/storage.h"
#include <memory>
#include <string>
#include <string_view>

namespace motor {

class dynamic_library {
public:
    dynamic_library(std::string_view name, storage& stg) noexcept;
    ~dynamic_library() noexcept;

    std::string get_name() const { return name; }

    bool exists() const;

    bool is_loaded() const;
    bool is_changed() const;

    bool load();
    void unload();

    void* get_symbol(std::string_view name) const;

private:
    struct impl;

    std::string name{};
    std::unique_ptr<impl> p{};
};

} // namespace motor

#endif // MOTOR_DYNAMIC_LIBRARY_H
