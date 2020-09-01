#include "loading_state.h"
#include "main_state.h"
#include <spdlog/spdlog.h>

namespace frontier {

void loading_state::on_start(entt::registry& reg) {
    spdlog::info("loading: start");
}

void loading_state::on_stop(entt::registry& reg) {
    spdlog::info("loading: stop");
}

motor::transition loading_state::update(entt::registry& reg) {
    progress++;
    spdlog::info("loading: {}", progress);
    if (progress >= 10) {
        return motor::transition_switch{std::make_shared<main_state>()};
    }
    return motor::transition_none{};
}

} // namespace frontier
