#ifndef REALMS_ENTRY_PLUGIN_HPP
#define REALMS_ENTRY_PLUGIN_HPP

#include <entt/signal/fwd.hpp>
#include <motor/app/fwd.hpp>
#include <motor/graphics/fwd.hpp>

namespace realms {

class entry_plugin {
public:
    explicit entry_plugin(motor::app_builder& app);

private:
    bool active{true};

    const motor::screen& screen;
    entt::dispatcher& dispatcher;

    void enter();
    void exit();

    void update_gui();
};

} // namespace realms

#endif // !REALMS_ENTRY_PLUGIN_HPP
