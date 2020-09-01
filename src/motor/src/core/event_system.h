#ifndef MOTOR_EVENT_SYSTEM_H
#define MOTOR_EVENT_SYSTEM_H

#include "motor/systems/system.h"

namespace motor {

class event_system : public system {
public:
    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    void update(entt::registry& reg) override;
};

} // namespace motor

#endif // MOTOR_EVENT_SYSTEM_H
