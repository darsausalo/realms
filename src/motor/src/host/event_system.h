#ifndef MOTOR_EVENT_SYSTEM_H
#define MOTOR_EVENT_SYSTEM_H

#include "motor/core/system.h"
#include <refl.hpp>

namespace motor {

class event_system : public system {
public:
    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    void update(entt::registry& reg) override;
};

} // namespace motor

REFL_AUTO(type(motor::event_system));

#endif // MOTOR_EVENT_SYSTEM_H
