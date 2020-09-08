#include "loading_state.h"
#include "main_state.h"
#include <motor/services/locator.h>
#include <motor/services/mods_service.h>
#include <spdlog/spdlog.h>

namespace frontier {

void loading_state::on_start(entt::registry& reg,
                             motor::system_dispatcher& disp) {
    spdlog::info("loading: start");

    motor::locator::mods::ref().load_plugins();
}

void loading_state::on_stop(entt::registry& reg,
                            motor::system_dispatcher& disp) {
    spdlog::info("loading: stop");
}

motor::transition loading_state::update(entt::registry& reg,
                                        motor::system_dispatcher& disp) {
    // progress++;
    // spdlog::info("loading: {}", progress);
    // if (progress >= 10) {
    //     return motor::transition_switch{std::make_shared<main_state>()};
    // }
    // return motor::transition_none{};

    return motor::transition_switch{std::make_shared<main_state>()};
}

} // namespace frontier
