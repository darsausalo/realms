#include "motor/core/application.h"
#include "core/config_system.h"
#include "core/event_system.h"
#include "core/window_system.h"
#include "motor/core/exception.h"
#include "motor/core/storage.h"
#include "motor/platform/platform.h"
#include "motor/systems/context.h"
#include "motor/systems/system_dispatcher.h"
#include <entt/entity/registry.hpp>
#include <fstream>
#include <mimalloc.h>
#include <spdlog/spdlog.h>

namespace motor {

application::application(int argc, const char* argv[]) {
    for (int i = 0; i < argc; i++) {
        if (argv[i]) {
            args.push_back(argv[i]);
        }
    }

    spdlog::info("{} v{} started", MOTOR_PROJECT_TITLE, MOTOR_PROJECT_VERSION);
    spdlog::info("mimalloc: {}", mi_version());
}

void application::run_loop(state_machine& states) {
    context ctx;
    system_dispatcher dispatcher{ctx};

    ctx.set<entt::dispatcher>()
            .sink<event::quit>()
            .connect<&application::receive_event_quit>(*this);
    ctx.set<storage>(platform.get_base_path(), platform.get_data_path(),
                     platform.get_user_path());

    dispatcher.add_system<config_system>(args);
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
