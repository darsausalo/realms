#include "motor/host/application.h"
#include "host/config_system.h"
#include "host/event_system.h"
#include "host/mods_system.h"
#include "host/window_system.h"
#include "motor/core/system_dispatcher.h"
#include "state_machine.h"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace motor {

application::application(int argc, const char* argv[]) {
    reg.set<arg_list>(argc, argv);
    reg.set<entt::dispatcher>()
            .sink<event::quit>()
            .connect<&application::receive_event_quit>(*this);
}

void application::run_loop(create_state_fn create_initial_state) {
    auto& dispatcher = reg.set<system_dispatcher>();

    dispatcher.add<system_group::pre_frame, config_system>(reg);
    dispatcher.add<system_group::pre_frame, window_system>(reg);
    dispatcher.add<system_group::pre_frame, mods_system>();
    dispatcher.add<system_group::pre_frame, event_system>(reg);

    state_machine states{reg, std::move(create_initial_state())};
    while (states.is_running() && !should_close()) {
        states.update();
        dispatcher.update();
    }
}

bool application::should_close() {
    return quit_requested;
}

} // namespace motor
