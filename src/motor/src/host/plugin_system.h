#ifndef MOTOR_PLUGIN_SYSTEM_H
#define MOTOR_PLUGIN_SYSTEM_H

#include "motor/core/system.h"
#include "motor/platform/dynamic_library.h"
#include <memory>
#include <refl.hpp>
#include <vector>

namespace motor {

class plugin_system : public system {
public:
    ~plugin_system() noexcept override;

    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    void update(entt::registry& reg) override;

private:
    // TODO: mods or plugins
    std::vector<std::unique_ptr<dynamic_library>> libs{};
};

} // namespace motor

REFL_AUTO(type(motor::plugin_system));

#endif // MOTOR_PLUGIN_SYSTEM_H
