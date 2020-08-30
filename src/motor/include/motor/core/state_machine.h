#ifndef MOTOR_STATE_MACHINE_H
#define MOTOR_STATE_MACHINE_H

#include "motor/core/transition.h"
#include <vector>

namespace motor {

class state_machine {
public:
    state_machine(std::shared_ptr<game_state> initial_state)
        : state_stack{initial_state} {}
    ~state_machine() noexcept = default;

    [[nodiscard]] bool is_running() noexcept { return running; }

    void start();
    void stop();
    void update();

private:
    bool running{};
    std::vector<std::shared_ptr<game_state>> state_stack;
};

} // namespace motor

#endif // MOTOR_STATE_MACHINE_H
