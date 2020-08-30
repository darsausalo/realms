#include "motor/core/application.h"
#include "platform/platform.h"
#include <spdlog/spdlog.h>

#include "motor/core/exception.h"

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
}

void application::shutdown() {
    spdlog::info("shutdown");
}

int application::run_game_loop() {
    states->start();
    while (states->is_running()) {
        states->update();
    }

    return 0;
}

} // namespace motor
