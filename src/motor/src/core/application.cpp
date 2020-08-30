#include "motor/core/application.h"
#include "core/event_system.h"
#include "core/window_system.h"
#include "motor/core/exception.h"
#include "motor/systems/game_data.h"
#include "motor/systems/system_dispatcher.h"
#include "platform/platform.h"
#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

namespace motor {

application::application(int argc, const char* argv[]) {
    platform::initialize();

    // TODO: use config
    spdlog::set_level(spdlog::level::debug);
}

application::~application() {
    platform::shutdown();
}

void application::run_game_loop(state_machine& states) {
    game_data data;
    system_dispatcher dispatcher{data};

    data.event_dispatcher.sink<event::quit>()
            .connect<&application::receive_event_quit>(*this);

    dispatcher.add_system<window_system>();
    dispatcher.add_system<event_system>();

    states.start();
    while (states.is_running()) {
        dispatcher.update();
        states.update();

        if (should_close()) {
            states.stop();
            break;
        }
    }
}

bool application::should_close() {
    return quit_requested;
}

} // namespace motor
