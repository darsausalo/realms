#ifndef MOTOR_EVENT_SYSTEM_H
#define MOTOR_EVENT_SYSTEM_H

#include "motor/core/events.h"
#include "motor/systems/system.h"

namespace motor {

class event_system : public system {
public:
    void on_start(game_data& data) override;
    void on_stop(game_data& data) override;
    void update(game_data& data) override;
};

} // namespace motor

#endif // MOTOR_EVENT_SYSTEM_H
