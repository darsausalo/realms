#ifndef MOTOR_PLUGIN_H
#define MOTOR_PLUGIN_H

#include "motor/host/plugin_context.h"
#include "motor/platform/dynamic_library.h"
#include <string>
#include <string_view>

namespace motor {

class plugin {
public:
    explicit plugin(std::string_view name) noexcept : name{name} {}
    ~plugin() noexcept = default;

private:
    std::string name;
    plugin_context ctx;
    dynamic_library dl;
};

} // namespace motor

#endif // MOTOR_PLUGIN_H
