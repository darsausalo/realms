#include "loading_state.h"
#include "component_serialization.h"
#include "frontier/components.h"
#include "main_state.h"
#include "motor/core/prototype_registry.h"
#include <chrono>
#include <entt/entity/registry.hpp>
#include <motor/services/components_service.h>
#include <motor/services/locator.h>
#include <motor/services/mods_service.h>
#include <motor/services/scripts_service.h>
#include <spdlog/spdlog.h>
#include <thread>

namespace frontier {

void loading_state::on_start(entt::registry& reg,
                             motor::system_dispatcher& disp) {
    spdlog::info("loading: start");

    motor::locator::components::ref()
            .component<position, velocity, health, sprite>();

    motor::locator::mods::ref().load_plugins();

    auto e1 = reg.create();
    reg.emplace<position>(e1, 1.0f, 2.0f);
    reg.emplace<velocity>(e1, 3.0f, 4.0f);

    auto e2 = reg.create();
    reg.emplace<position>(e2, 4.0f, 5.0f);
    reg.emplace<velocity>(e2, 6.0f, 7.0f);

    // TODO: exception handling in async

    loading_f = std::async(std::launch::async, [&reg] {
        sol::state lua{};
        reg.set<motor::prototype_registry>(
                motor::locator::scripts::ref().load_prototypes(lua));

        auto& protos = reg.ctx<motor::prototype_registry>();
        if (protos.get("soldier"_hs) != entt::null) {
            spdlog::debug("EXISTS!");
        } else {
            auto id = entt::hashed_string::value("soldier");
            spdlog::debug("NOT FOUND: {}({})", "soldier", id);
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(0.1s);
    });

    spdlog::info("loading: started {}", reg.view<position, velocity>().size());
}

void loading_state::on_stop(entt::registry& reg,
                            motor::system_dispatcher& disp) {
    spdlog::info("loading: stop");
}

motor::transition loading_state::update(entt::registry& reg,
                                        motor::system_dispatcher& disp) {
    using namespace std::chrono_literals;
    if (loading_f.wait_for(0s) == std::future_status::ready) {
        loading_f.get(); // TODO: exception handling in async?
        spdlog::debug("done");
        return motor::transition_switch{std::make_shared<main_state>()};
    }

    return motor::transition_none{};
}

} // namespace frontier
