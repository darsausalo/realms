#include "frontier/states/main_state.h"
#include <motor/services/locator.h>
#include <motor/services/mods_service.h>
#include <spdlog/spdlog.h>

namespace frontier {

void main_state::on_start(entt::registry& reg, motor::system_dispatcher& disp) {
    spdlog::info("game: start");
    motor::locator::mods::ref().start_plugins(disp);
}

void main_state::on_stop(entt::registry& reg, motor::system_dispatcher& disp) {
    motor::locator::mods::ref().stop_plugins(disp);
    spdlog::info("game: stop");
}

motor::transition main_state::update(entt::registry& reg,
                                     motor::system_dispatcher& disp) {
    motor::locator::mods::ref().reload_plugins(disp);

    return motor::transition_none{};
}

} // namespace frontier
