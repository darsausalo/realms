#include "frontier/states/startup_state.hpp"
#include "frontier/states/game_state.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace frontier {

startup_state::startup_state(entt::registry& reg) : motor::state{reg} {
    reg.ctx<entt::dispatcher>()
            .sink<motor::event::start>()
            .connect<&startup_state::receive_start>(*this);
}

motor::transition startup_state::update() {
    if (started) {
        return motor::transition_switch{std::make_shared<game_state>(reg)};
    }

    return motor::transition_none{};
}

} // namespace frontier
