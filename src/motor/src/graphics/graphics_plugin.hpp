#ifndef MOTOR_GRAPHICS_PLUGIN_HPP
#define MOTOR_GRAPHICS_PLUGIN_HPP

#include "motor/graphics/screen.hpp"

namespace motor {

class app_builder;

class graphics_plugin {
public:
    graphics_plugin(app_builder& app);
    ~graphics_plugin();

private:
    screen& screen;

    void pre_render();
    void post_render();
};

} // namespace motor

#endif // MOTOR_GRAPHICS_PLUGIN_HPP
