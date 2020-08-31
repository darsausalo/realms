#ifndef MOTOR_EVENT_SYSTEM_H
#define MOTOR_EVENT_SYSTEM_H

#include "motor/core/events.h"
#include "motor/systems/system.h"

namespace motor {

class event_system : public system {
public:
    void on_start(context& ctx) override;
    void on_stop(context& ctx) override;
    void update(context& ctx) override;
};

} // namespace motor

#endif // MOTOR_EVENT_SYSTEM_H
