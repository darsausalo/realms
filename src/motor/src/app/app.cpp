#include "motor/app/app.hpp"
#include "host/config_system.h"
#include "host/event_system.h"
#include "host/mods_system.h"
#include "host/state_machine.h"
#include "host/window_system.h"
#include "motor/core/prototype_registry.h"
#include "motor/core/system_dispatcher.h"
#include "motor/entity/components.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace motor {

app::app(int argc, const char* argv[]) {
    components::define<prototype>();

    reg.set<arg_list>(argc, argv);
    reg.set<entt::dispatcher>()
            .sink<event::quit>()
            .connect<&app::receive_event_quit>(*this);
    reg.set<prototype_registry>();
}

void app::run_loop(create_state_fn create_initial_state) {
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

bool app::should_close() {
    return quit_requested;
}

} // namespace motor
