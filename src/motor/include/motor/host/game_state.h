#ifndef MOTOR_GAME_STATE_H
#define MOTOR_GAME_STATE_H

#include "motor/host/transition.h"
#include <entt/entity/fwd.hpp>

namespace motor {

class game_state {
public:
    game_state() noexcept = default;
    virtual ~game_state() noexcept = default;

    virtual void on_start(entt::registry& reg) {}
    virtual void on_stop(entt::registry& reg) {}
    virtual transition update(entt::registry& reg) { return transition_none{}; }
};

} // namespace motor

#endif // MOTOR_GAME_STATE_H
