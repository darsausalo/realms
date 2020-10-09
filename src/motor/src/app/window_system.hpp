#ifndef MOTOR_WINDOW_SYSTEM_HPP
#define MOTOR_WINDOW_SYSTEM_HPP

#include "motor/graphics/screen.hpp"
#include <SDL.h>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace motor {

class app_builder;

struct window_position {
    int x;
    int y;
};

struct window_size {
    int width;
    int height;
};

struct window_config {
    std::string monitor;
    bool fullscreen;
    window_position position;
    window_size size;
    bool vsync;
};

class window_system {
public:
    window_system(app_builder& app);
    ~window_system();

private:
    entt::dispatcher& dispatcher;
    screen& screen;
    nlohmann::json& jconfig;
    window_config config;
    SDL_Window* window{};
    SDL_GLContext ctx;

    void update_position();
    void poll_events();
    void swap_buffers();
};

} // namespace motor

#endif // MOTOR_WINDOW_SYSTEM_HPP
