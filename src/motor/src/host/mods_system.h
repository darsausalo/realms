#ifndef MOTOR_MODS_SYSTEM_H
#define MOTOR_MODS_SYSTEM_H

#include "motor/core/system.h"
#include "motor/core/type_traits.h"
#include <memory>
#include <refl.hpp>
#include <vector>

namespace motor {

class mods_system : public init_system {
public:
    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
};

template<>
struct is_host_type<mods_system> : std::true_type {};

} // namespace motor

REFL_AUTO(type(motor::mods_system));

#endif // MOTOR_MODS_SYSTEM_H
