#include "frontier/states/startup_state.hpp"
#include "frontier/states/game_state.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace frontier {

void startup_state::on_start() {
    dispatcher.sink<motor::event::start>()
            .connect<&startup_state::receive_start>(*this);
}

void startup_state::on_stop() {
    dispatcher.sink<motor::event::start>().disconnect(*this);
}

motor::transition startup_state::update() {
    if (started) {
        return motor::transition_switch{std::make_shared<game_state>(app)};
    }

    return motor::transition_none{};
}

} // namespace frontier
