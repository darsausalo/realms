#include "frontier/states/loading_state.h"
#include "components/serialization.h"
#include "frontier/components/base.h"
#include "frontier/states/main_state.h"
#include "motor/core/prototype_registry.h"
#include <chrono>
#include <entt/entity/registry.hpp>
#include <motor/services/components_service.h>
#include <motor/services/locator.h>
#include <motor/services/mods_service.h>
#include <motor/services/scripts_service.h>
#include <spdlog/spdlog.h>

namespace frontier {

void loading_state::on_start(entt::registry& reg,
                             motor::system_dispatcher& disp) {
    motor::locator::components::ref()
            .component<position, velocity, health, sprite>();

    motor::locator::components::ref().prepare(reg);

    motor::locator::mods::ref().load_plugins();

    thread = std::thread([this, &reg] {
        progress.update("loading prototypes");

        sol::state lua{};
        reg.set<motor::prototype_registry>().transpire(
                motor::locator::scripts::ref().load_prototypes(lua));

        // TODO: remove --->>>
        auto& protos = reg.ctx<motor::prototype_registry>();
        if (protos.get("soldier"_hs) != entt::null) {
            spdlog::debug("EXISTS!");
        } else {
            auto id = entt::hashed_string::value("soldier");
            spdlog::debug("NOT FOUND: {}({})", "soldier", id);
        }

        // using namespace std::chrono_literals;
        // std::this_thread::sleep_for(2.1s);
        // TODO: remove <<<---

        progress.complete();
    });
}

void loading_state::on_stop(entt::registry& reg,
                            motor::system_dispatcher& disp) {
    if (thread.joinable()) {
        thread.join();
    }
}

motor::transition loading_state::update(entt::registry& reg,
                                        motor::system_dispatcher& disp) {
    if (progress.is_completed()) {
        if (thread.joinable()) {
            thread.join();
        }
        return motor::transition_switch{std::make_shared<main_state>()};
    }

    return motor::transition_none{};
}

} // namespace frontier