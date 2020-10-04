#ifndef MOTOR_APP_HPP
#define MOTOR_APP_HPP

#include "motor/core/event.hpp"
#include "motor/host/state.h"
#include <entt/entity/registry.hpp>

namespace motor {

class app {
    using create_state_fn = std::function<std::shared_ptr<state>()>;

public:
    app(const app&) = delete;
    app(app&&) = delete;

    app& operator=(const app&) = delete;
    app& operator=(app&&) = delete;

    template<typename InitialState>
    int run() {
        run_loop([this]() {
            return std::move(std::make_shared<InitialState>(reg));
        });
        return 0;
    }

protected:
    app(int argc, const char* argv[]);

private:
    entt::registry reg{};
    bool quit_requested{};

    void run_loop(create_state_fn create_initial_state);

    bool should_close();

    void receive_quit(const event::quit&) { quit_requested = true; }
};

} // namespace motor

#endif // MOTOR_APP_HPP
