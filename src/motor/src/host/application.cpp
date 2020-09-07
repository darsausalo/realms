#include "motor/host/application.h"
#include "host/config_system.h"
#include "host/event_system.h"
#include "host/window_system.h"
#include "motor/core/system_dispatcher.h"
#include "motor/host/storage.h"
#include "platform/platform.h"
#include "state_machine.h"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <fstream>
#include <spdlog/spdlog.h>

namespace motor {

application::application(int argc, const char* argv[]) {
    auto& args = reg.set<arg_list>();
    for (int i = 0; i < argc; i++) {
        if (argv[i]) {
            args.push_back(argv[i]);
        }
    }

    reg.set<entt::dispatcher>()
            .sink<event::quit>()
            .connect<&application::receive_event_quit>(*this);
}

void application::run_loop(std::shared_ptr<game_state>&& initial_state) {
    system_dispatcher dispatcher{reg};

    dispatcher.add_system<config_system>();
    dispatcher.add_system<window_system>();
    dispatcher.add_system<event_system>();

    state_machine states{reg, std::move(initial_state)};
    while (states.is_running() && !should_close()) {
        dispatcher.update();
        states.update();
    }
}

bool application::should_close() {
    return quit_requested;
}

} // namespace motor
