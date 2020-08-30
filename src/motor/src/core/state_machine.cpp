#include "motor/core/state_machine.h"
#include "motor/core/exception.h"
#include "motor/core/game_state.h"

namespace motor {

void state_machine::start() {
    if (state_stack.empty()) {
        throw exception("Initial state not found");
    }
    auto state = state_stack.back();
    state->on_start();
    running = true;
}

void state_machine::stop() {
    if (running) {
        while (!state_stack.empty()) {
            auto state = state_stack.back();
            state->on_stop();

            state_stack.pop_back();
        }
    }
    running = false;
}

void state_machine::update() {
    if (running) {
        auto state = state_stack.back();
        auto trans = state->update();

        if (auto t_quit = std::get_if<transition_quit>(&trans); t_quit) {
            state->on_stop();
            running = false;
        } else if (auto t_switch = std::get_if<transition_switch>(&trans);
                   t_switch) {
            state->on_stop();
            state_stack.pop_back();
            state_stack.push_back(t_switch->game_state);
        }
    }
}

} // namespace motor
