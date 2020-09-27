#include "loading_state.h"
#include "frontier/components.h"
#include "main_state.h"
#include <chrono>
#include <entt/entity/registry.hpp>
#include <motor/services/locator.h>
#include <motor/services/mods_service.h>
#include <motor/services/scripts_service.h>
#include <spdlog/spdlog.h>
#include <thread>

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

    reg.set<motor::template_set>();

    loading_f = std::async(std::launch::async, [&reg] {
        motor::locator::scripts::ref().run_scripts();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5s);
    });

    spdlog::info("loading finished: {}", reg.view<position, velocity>().size());
}

void loading_state::on_stop(entt::registry& reg,
                            motor::system_dispatcher& disp) {
    spdlog::info("loading: stop");
}

motor::transition loading_state::update(entt::registry& reg,
                                        motor::system_dispatcher& disp) {
    using namespace std::chrono_literals;
    if (loading_f.wait_for(0s) == std::future_status::ready) {
        spdlog::debug("done");
        return motor::transition_switch{std::make_shared<main_state>()};
    }

    return motor::transition_none{};
}

} // namespace frontier
