#ifndef MOTOR_APPLICATION_H
#define MOTOR_APPLICATION_H

#include "motor/core/events.h"
#include "motor/core/state_machine.h"
#include "motor/systems/game_data.h"
#include "motor/systems/system_dispatcher.h"
#include <string>

namespace motor {

class application final {
public:
    application(int argc, const char* argv[]);
    ~application();

    template<typename InitialState>
    int run() {
        states = std::make_unique<state_machine>(
                std::make_shared<InitialState>());

        initialize();
        auto result = run_game_loop();
        shutdown();

        return result;
    }

private:
    std::vector<std::string> args{};
    std::unique_ptr<state_machine> states{};
    game_data data{};
    system_dispatcher dispatcher{};
    bool quit_requested{};

    void initialize();
    void shutdown();
    int run_game_loop();

    bool should_close();

    void receive_event_quit(const event::quit&) { quit_requested = true; }
};

} // namespace motor

#endif // MOTOR_APPLICATION_H
