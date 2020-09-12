#include "loading_state.h"
#include "frontier/components.h"
#include "main_state.h"
#include <entt/entity/registry.hpp>
#include <motor/services/locator.h>
#include <motor/services/mods_service.h>
#include <spdlog/spdlog.h>

namespace frontier {

void loading_state::on_start(entt::registry& reg,
                             motor::system_dispatcher& disp) {
    spdlog::info("loading: start");

    motor::locator::mods::ref().load_plugins();

    auto e1 = reg.create();
    reg.emplace<position>(e1, 1.0f, 2.0f);
    reg.emplace<velocity>(e1, 3.0f, 4.0f);

    auto e2 = reg.create();
    reg.emplace<position>(e2, 4.0f, 5.0f);
    reg.emplace<velocity>(e2, 6.0f, 7.0f);

    spdlog::info("loading finished: {}", reg.view<position, velocity>().size());
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
