#ifndef MOTOR_APPLICATION_H
#define MOTOR_APPLICATION_H

#include "motor/host/state.h"
#include <entt/entity/registry.hpp>

namespace motor {

namespace event {
struct quit {};
} // namespace event

class application final {
    using create_state_fn = std::function<std::shared_ptr<state>()>;

public:
    application(int argc, const char* argv[]);
    application(const application&) = delete;
    application(application&&) = delete;

    application& operator=(const application&) = delete;
    application& operator=(application&&) = delete;

    template<typename InitialState>
    int run() {
        run_loop([this]() {
            return std::move(std::make_shared<InitialState>(reg));
        });
        return 0;
    }

private:
    entt::registry reg{};
    bool quit_requested{};

    void run_loop(create_state_fn create_initial_state);

    bool should_close();

    void receive_event_quit(const event::quit&) { quit_requested = true; }
};

} // namespace motor

#endif // MOTOR_APPLICATION_H
