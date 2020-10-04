#ifndef MOTOR_STATE_MACHINE_H
#define MOTOR_STATE_MACHINE_H

#include "motor/host/state.h"
#include "motor/host/transition.h"
#include <entt/entity/fwd.hpp>
#include <vector>

namespace motor {

class system_dispatcher;

class state_machine {
public:
    state_machine(entt::registry& reg, std::shared_ptr<state>&& initial_state);
    ~state_machine();

    [[nodiscard]] bool is_running() noexcept { return running; }

    void update();

private:
    bool running;

    entt::registry& reg;
    std::vector<std::shared_ptr<state>> state_stack{};
};

} // namespace motor

#endif // MOTOR_STATE_MACHINE_H
