#include "frontier/states/loading_state.h"
#include "frontier/states/main_state.h"
#include "motor/core/prototype_registry.h"
#include <chrono>
#include <entt/entity/registry.hpp>
#include <motor/services/locator.h>
#include <motor/services/scripts_service.h>
#include <spdlog/spdlog.h>

namespace frontier {

loading_state::loading_state(entt::registry& reg) : motor::state{reg} {
    thread = std::thread([this, &reg] {
        progress.update("loading prototypes");

        sol::state lua{};
        reg.ctx<motor::prototype_registry>().transpire(
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

loading_state::~loading_state() {
    if (thread.joinable()) {
        thread.join();
    }
}

motor::transition loading_state::update() {
    if (progress.is_completed()) {
        if (thread.joinable()) {
            thread.join();
        }
        return motor::transition_switch{std::make_shared<main_state>(reg)};
    }

    return motor::transition_none{};
}

} // namespace frontier
