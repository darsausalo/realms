#ifndef MOTOR_GAME_STATE_H
#define MOTOR_GAME_STATE_H

#include "motor/core/transition.h"

namespace motor {

class game_state {
public:
    game_state() noexcept = default;
    virtual ~game_state() noexcept = default;

    virtual void on_start() {}
    virtual void on_stop() {}
    virtual transition update() { return transition_none{}; }
};

} // namespace motor

#endif // MOTOR_GAME_STATE_H
