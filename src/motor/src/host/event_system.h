#ifndef MOTOR_EVENT_SYSTEM_H
#define MOTOR_EVENT_SYSTEM_H

#include "motor/core/system.h"
#include "motor/core/type_traits.h"
#include <entt/entity/registry.hpp>

namespace motor {

class event_system {
public:
    event_system(entt::registry& reg) noexcept;
    ~event_system() noexcept;

    void operator()();

private:
    entt::registry& reg;
};

} // namespace motor

#endif // MOTOR_EVENT_SYSTEM_H
