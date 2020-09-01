#ifndef MOTOR_APPLICATION_H
#define MOTOR_APPLICATION_H

#include "motor/core/events.h"
#include "motor/core/state_machine.h"
#include "motor/systems/system_dispatcher.h"
#include <memory>
#include <string>

namespace motor {

class platform;

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
        run_loop(state_machine(std::make_shared<InitialState>()));
        return 0;
    }

private:
    std::vector<std::string> args;
    std::unique_ptr<platform> platform;
    bool quit_requested{};

    void run_loop(state_machine& states);

    bool should_close();

    void receive_event_quit(const event::quit&) { quit_requested = true; }
};

} // namespace motor

#endif // MOTOR_APPLICATION_H
