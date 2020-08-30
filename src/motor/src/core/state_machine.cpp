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

        std::visit(
                [this, state](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, transition_quit>) {
                        state->on_stop();
                        running = false;
                    }
                },
                trans);
    }
}

} // namespace motor
