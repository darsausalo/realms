#ifndef MOTOR_GUI_PLUGIN_HPP
#define MOTOR_GUI_PLUGIN_HPP

#include "motor/graphics/screen.hpp"
#include <SDL.h>

namespace motor {

class app_builder;

class gui_plugin {
public:
    explicit gui_plugin(app_builder& app);
    ~gui_plugin();

private:
    screen& screen;

    void handle_input();
    void draw();

    SDL_Window* window;
};

} // namespace motor

#endif // MOTOR_GUI_PLUGIN_HPP
