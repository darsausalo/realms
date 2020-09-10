#ifndef MOTOR_EVENT_SYSTEM_H
#define MOTOR_EVENT_SYSTEM_H

#include "motor/core/system.h"
#include "motor/core/type_traits.h"
#include <refl.hpp>

namespace motor {

class event_system : public init_system {
public:
    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    void update(entt::registry& reg) override;
};

template<>
struct is_host_type<event_system> : std::true_type {};

} // namespace motor

REFL_AUTO(type(motor::event_system));

#endif // MOTOR_EVENT_SYSTEM_H
