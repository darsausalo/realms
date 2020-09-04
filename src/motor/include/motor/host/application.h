#ifndef MOTOR_APPLICATION_H
#define MOTOR_APPLICATION_H

#include "motor/core/system_dispatcher.h"
#include "motor/host/game_state.h"
#include <entt/entity/registry.hpp>
#include <memory>
#include <string>

namespace motor {

namespace event {
struct quit {};
} // namespace event

class application final {
public:
    application(int argc, const char* argv[]);
    application(const application&) = delete;
    application(application&&) = delete;

    application& operator=(const application&) = delete;
    application& operator=(application&&) = delete;

    ~application();

    template<typename InitialState>
    int run() {
        run_loop(std::move(std::make_shared<InitialState>()));
        return 0;
    }

private:
    entt::registry reg{};
    bool quit_requested{};

    void run_loop(std::shared_ptr<game_state>&& initial_state);

    bool should_close();

    void receive_event_quit(const event::quit&) { quit_requested = true; }
};

} // namespace motor

#endif // MOTOR_APPLICATION_H
