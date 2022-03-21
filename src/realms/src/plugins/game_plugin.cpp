#include "game_plugin.hpp"
#include "components/base.hpp"
#include "components/registration.hpp"
#include "events.hpp"
#include "systems/registration.hpp"
#include <entt/signal/dispatcher.hpp>
#include <fmt/format.h>
#include <glm/vec2.hpp>
#include <imgui.h>
#include <motor/app/app_builder.hpp>
#include <motor/core/input.hpp>
#include <motor/core/time.hpp>
#include <motor/entity/transform.hpp>
#include <motor/graphics/screen.hpp>

namespace realms {

using namespace entt::literals;

void apply_player_input(
    entt::view<entt::get_t<const speed, velocity, entt::tag<"player"_hs>>> view,
    const motor::input_axises& input) {
    auto axis = input.get_value("move"_hs);
    for (auto&& [_, s, v] : view.each()) {
        v.dx = axis.x * s.value;
        v.dy = axis.y * s.value;
    }
}

game_plugin::game_plugin(motor::app_builder& app)
    : registry{app.registry()}
    , dispatcher{app.dispatcher()}
    , game{app.registry().ctx_or_set<game_context>()}
    , screen{app.registry().ctx<motor::screen>()}
    , prototypes{app.registry().ctx<motor::prototype_registry>()} {
    dispatcher.sink<event::start_game>().connect<&game_plugin::enter>(*this);

    // TODO: move to player systems group
    app.add_system<&apply_player_input>();

    register_components(app);
    register_systems(app);
}

void game_plugin::enter() {
    game.state = "game"_hs;

    registry.clear();

    auto e = prototypes.spawn(registry, "player"_hs);
    if (e != entt::null) {
        std::string info{"spawned player with:\n"};
        for (auto&& [_, storage] : registry.storage()) {
            if (storage.contains(e)) {
                info += "  ";
                info += storage.type().name();
                info += "\n";
            }
        }
        spdlog::debug(info);
    } else {
        spdlog::error("failed to spawn player");
    }
}

} // namespace realms
