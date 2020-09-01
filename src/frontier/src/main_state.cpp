#include "main_state.h"
#include <spdlog/spdlog.h>

namespace frontier {

void main_state::on_start(entt::registry& reg) {
    spdlog::info("game: start");
}

void main_state::on_stop(entt::registry& reg) {
    spdlog::info("game: stop");
}

motor::transition main_state::update(entt::registry& reg) {
    return motor::transition_none{};
}

} // namespace frontier
