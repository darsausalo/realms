#include "motor/app/app.hpp"
#include "app/config_system.hpp"
#include "app/event_system.hpp"
#include "app/input_system.hpp"
#include "app/state_machine.hpp"
#include "app/time_system.hpp"
#include "app/window_system.hpp"
#include "mods/mods_system.hpp"
#include "motor/entity/components.hpp"
#include "motor/entity/prototype_registry.hpp"
#include "motor/entity/system_dispatcher.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace motor {

app::app(int argc, const char* argv[]) {
    components::define<prototype>();
    components::define<timer>();

    registry.set<entt::dispatcher>() //
            .sink<event::quit>()     //
            .connect<&app::receive_quit>(*this);
    registry.set<prototype_registry>();

    add_system<config_system, stage::NONE>(arg_list{argc, argv}, registry);
    add_system<window_system, stage::PRE_FRAME>(registry);
    add_system<mods_system, stage::PRE_FRAME>(registry);
    add_system<time_system, stage::PRE_EVENT>(registry);
    add_system<timer_system, stage::PRE_EVENT>(registry);
    add_system<input_system, stage::PRE_EVENT>(registry);
    add_system<event_system, stage::ON_EVENT>(registry);
}

void app::run_loop(std::shared_ptr<state>&& initial_state) {
    state_machine states{std::move(initial_state)};
    while (states.is_running() && !should_close()) {
        states.update();
        dispatcher.update();
    }
}

bool app::should_close() {
    return quit_requested;
}

} // namespace motor
