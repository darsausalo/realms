#ifndef REALMS_ENTRY_PLUGIN_HPP
#define REALMS_ENTRY_PLUGIN_HPP

#include "game.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <motor/app/fwd.hpp>
#include <motor/graphics/fwd.hpp>

namespace realms {

class entry_plugin {
public:
    explicit entry_plugin(motor::app_builder& app);

private:
    entt::registry& registry;
    entt::dispatcher& dispatcher;
    game_context& game;
    const motor::screen& screen;

    void enter();

    void update_gui();
};

} // namespace realms

#endif // !REALMS_ENTRY_PLUGIN_HPP
