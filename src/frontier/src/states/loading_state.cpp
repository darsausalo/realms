#include "frontier/states/loading_state.h"
#include "frontier/states/main_state.h"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace frontier {

loading_state::loading_state(entt::registry& reg) : motor::state{reg} {
    reg.ctx<entt::dispatcher>()
            .sink<motor::event::start>()
            .connect<&loading_state::receive_start>(*this);
}

motor::transition loading_state::update() {
    if (started) {
        return motor::transition_switch{std::make_shared<main_state>(reg)};
    }

    return motor::transition_none{};
}

} // namespace frontier
