#ifndef MOTOR_STATE_MACHINE_H
#define MOTOR_STATE_MACHINE_H

#include "motor/core/game_state.h"
#include "motor/core/transition.h"
#include <entt/entity/fwd.hpp>
#include <vector>

namespace motor {

class state_machine {
public:
    state_machine(entt::registry& reg,
                  const std::shared_ptr<game_state>&& initial_state);
    ~state_machine();

    [[nodiscard]] bool is_running() noexcept { return running; }

    void update();

private:
    bool running;

    entt::registry& reg;
    std::vector<std::shared_ptr<game_state>> state_stack;
};

} // namespace motor

#endif // MOTOR_STATE_MACHINE_H
