#ifndef MOTOR_MODS_SYSTEM_H
#define MOTOR_MODS_SYSTEM_H

#include "motor/core/system.h"
#include "motor/core/type_traits.h"
#include <memory>
#include <vector>

namespace motor {

class mods_system : public system<system_group::post_load> {
public:
    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
};

} // namespace motor

#endif // MOTOR_MODS_SYSTEM_H
