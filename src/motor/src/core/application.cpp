#include "motor/core/application.h"
#include "core/config_system.h"
#include "core/event_system.h"
#include "core/window_system.h"
#include "motor/core/core_context.h"
#include "motor/core/storage.h"
#include "motor/systems/system_dispatcher.h"
#include "platform/platform.h"
#include "state_machine.h"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <fstream>
#include <mimalloc.h>
#include <spdlog/spdlog.h>

namespace motor {

application::application(int argc, const char* argv[])
    : platform(new ::motor::platform{}) {
    for (int i = 0; i < argc; i++) {
        if (argv[i]) {
            args.push_back(argv[i]);
        }
    }

    spdlog::info("{} v{} started", MOTOR_PROJECT_TITLE, MOTOR_PROJECT_VERSION);
    spdlog::info("mimalloc: {}", mi_version());
}

application::~application() {
}

void application::run_loop(std::shared_ptr<game_state>&& initial_state) {
    entt::registry reg;

    reg.set<entt::dispatcher>()
            .sink<event::quit>()
            .connect<&application::receive_event_quit>(*this);
    reg.set<core_context>(platform->get_base_path(), platform->get_data_path(),
                          platform->get_user_path());

    system_dispatcher dispatcher{reg};

    dispatcher.add_system<config_system>(args);
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
