#ifndef MOTOR_TRANSITION_H
#define MOTOR_TRANSITION_H

#include <memory>
#include <variant>

namespace motor {

class game_state;

struct transition_none {};

struct transition_quit {};

struct transition_push {
    std::shared_ptr<game_state> game_state;
};

struct transition_switch {
    std::shared_ptr<game_state> game_state;
};

using transition = std::variant<transition_none, transition_quit,
                                transition_push, transition_switch>;

} // namespace motor

#endif // MOTOR_TRANSITION_H
