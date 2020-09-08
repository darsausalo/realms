#ifndef MOTOR_PLUGIN_CONTEXT_H
#define MOTOR_PLUGIN_CONTEXT_H

#include "motor/core/system_module.h"

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

    virtual std::shared_ptr<spdlog::logger> get_logger() const = 0;

protected:
    plugin_context() noexcept = default;

    virtual void
    add_module(std::string_view module_name,
               std::unique_ptr<system_module>&& module_instance) = 0;
};

} // namespace motor

#endif // MOTOR_PLUGIN_CONTEXT_H
