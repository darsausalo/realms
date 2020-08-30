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
        run_game_loop(state_machine(std::make_shared<InitialState>()));

        return 0;
    }

private:
    bool quit_requested{};

    void run_game_loop(state_machine& states);

    bool should_close();

    void receive_event_quit(const event::quit&) { quit_requested = true; }
};

} // namespace motor

#endif // MOTOR_APPLICATION_H
