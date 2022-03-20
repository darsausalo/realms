#ifndef REALMS_GAME_PLUGIN_HPP
#define REALMS_GAME_PLUGIN_HPP

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <motor/app/fwd.hpp>
#include <motor/graphics/fwd.hpp>

namespace realms {

class game_plugin {
public:
    explicit game_plugin(motor::app_builder& app);

private:
    bool active{false};

    entt::registry& registry;

    const motor::screen& screen;
    entt::dispatcher& dispatcher;

    void enter();
    void exit();

    void update_gui();
};

} // namespace realms


#endif // !REALMS_GAME_PLUGIN_HPP
