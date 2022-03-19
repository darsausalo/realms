#ifndef MOTOR_GUI_PLUGIN_HPP
#define MOTOR_GUI_PLUGIN_HPP

#include "motor/graphics/screen.hpp"
#include <SDL.h>
#include <string>

namespace motor {

class app_builder;

struct gui_config {
    std::string default_font;
    std::size_t default_font_size;
};

class gui_plugin {
public:
    explicit gui_plugin(app_builder& app);
    ~gui_plugin();

private:
    screen& screen;
    gui_config config;

    SDL_Window* window;

    void begin_frame();
    void end_frame();
};

} // namespace motor

#endif // MOTOR_GUI_PLUGIN_HPP
