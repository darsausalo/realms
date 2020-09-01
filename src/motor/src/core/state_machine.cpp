#include "state_machine.h"
#include "motor/core/exception.h"
#include "motor/core/game_state.h"

namespace motor {

state_machine::state_machine(entt::registry& reg,
                             const std::shared_ptr<game_state>&& initial_state)
    : reg{reg}, running{true}, state_stack{std::move(initial_state)} {
    state_stack.back()->on_start(reg);
}

state_machine::~state_machine() {
    while (!state_stack.empty()) {
        auto state = state_stack.back();
        state->on_stop(reg);

        state_stack.pop_back();
    }
}

void state_machine::update() {
    auto state = state_stack.back();
    auto trans = state->update(reg);

    if (auto t_quit = std::get_if<transition_quit>(&trans); t_quit) {
        state->on_stop(reg);
        running = false;
    } else if (auto t_switch = std::get_if<transition_switch>(&trans);
               t_switch) {
        state->on_stop(reg);
        state_stack.pop_back();
        state_stack.push_back(t_switch->game_state);
        state_stack.back()->on_start(reg);
    }
}

} // namespace motor
