#ifndef REALMS_GAME_PLUGIN_HPP
#define REALMS_GAME_PLUGIN_HPP

#include "../components/base.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <motor/app/fwd.hpp>
#include <motor/entity/fwd.hpp>
#include <motor/graphics/fwd.hpp>

namespace realms {

class game_plugin {
public:
    explicit game_plugin(motor::app_builder& app);

private:
    entt::registry& registry;
    entt::dispatcher& dispatcher;
    motor::app_state& app_state;
    const motor::screen& screen;
    motor::prototype_registry& prototypes;

    void enter();
};

} // namespace realms


#endif // !REALMS_GAME_PLUGIN_HPP
