#include "motor/core/application.h"
#include "core/event_system.h"
#include "core/window_system.h"
#include "motor/core/exception.h"
#include "motor/systems/game_data.h"
#include "motor/systems/system_dispatcher.h"
#include "platform/platform.h"
#include <spdlog/spdlog.h>

namespace motor {

static void init_args(int argc, const char* argv[],
                      std::vector<std::string> args) {
    for (int i = 0; i < argc; i++) {
        args.push_back(argv[i]);
    }
}

application::application(int argc, const char* argv[]) {
    init_args(argc, argv, args);
    platform::initialize();
}

application::~application() {
    platform::shutdown();
}

void application::initialize() {
    spdlog::info("initialize");

    data.event_dispatcher.sink<event::quit>()
            .connect<&application::receive_event_quit>(*this);

    dispatcher.add_system<window_system>();
    dispatcher.add_system<event_system>();
}

void application::shutdown() {
    spdlog::info("shutdown");
}

int application::run_game_loop() {
    dispatcher.start(data);
    states->start();
    while (states->is_running()) {
        dispatcher.update(data);
        states->update();

        if (should_close()) {
            states->stop();
            break;
        }
    }
    dispatcher.stop(data);

    return 0;
}

bool application::should_close() {
    return quit_requested;
}

} // namespace motor
