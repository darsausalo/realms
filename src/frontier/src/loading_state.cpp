#include "loading_state.h"
#include <spdlog/spdlog.h>

namespace frontier {

void loading_state::on_start() {
    spdlog::info("loading: start");
}

void loading_state::on_stop() {
    spdlog::info("loading: stop");
}

motor::transition loading_state::update() {
    progress++;
    spdlog::info("loading: {}", progress);
    if (progress >= 10) {
        return motor::transition_quit{};
    }
    return motor::transition_none{};
}

} // namespace frontier
