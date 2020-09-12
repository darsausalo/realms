#ifndef MOTOR_PLUGIN_CONTEXT_H
#define MOTOR_PLUGIN_CONTEXT_H

#include "motor/core/attribute.h"
#include "motor/core/system_dispatcher.h"
#include <functional>
#include <spdlog/spdlog.h>

namespace motor {

class plugin_context {
public:
    virtual ~plugin_context() noexcept = default;

    template<typename Module>
    void module() {
        static_assert(
                std::is_base_of_v<system_module, Module>,
                "System module should be derived from motor::system_module");
        add_module(nameof_type<Module>(), std::make_unique<Module>());
    }

    template<typename System, typename... Dependencies>
    void system() noexcept {
        system_adders.push_back(
                [](auto& d) { d.add_system<System, Dependencies...>(); });
        system_removers.push_back([](auto& d) { d.remove_system<System>(); });
    }

    virtual std::shared_ptr<spdlog::logger> get_logger() const = 0;

protected:
    plugin_context() noexcept = default;

    using system_func = std::function<void(system_dispatcher&)>;

    std::vector<system_func> system_adders;
    std::vector<system_func> system_removers;
};

} // namespace motor

#endif // MOTOR_PLUGIN_CONTEXT_H
