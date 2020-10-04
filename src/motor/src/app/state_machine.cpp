#include "state_machine.h"
#include "motor/host/state.h"

namespace motor {

state_machine::state_machine(entt::registry& reg,
                             std::shared_ptr<state>&& initial_state)
    : reg{reg}, running{true}, state_stack{std::move(initial_state)} {
}

state_machine::~state_machine() {
    while (!state_stack.empty()) {
        state_stack.pop_back();
    }
}

void state_machine::update() {
    auto& state = state_stack.back();
    auto trans = state->update();

    if (auto t_quit = std::get_if<transition_quit>(&trans); t_quit) {
        running = false;
    } else if (auto t_switch = std::get_if<transition_switch>(&trans);
               t_switch) {
        state_stack.pop_back();
        state_stack.push_back(t_switch->state);
    }
}

} // namespace motor
