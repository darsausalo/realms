#ifndef MOTOR_WINDOW_SYSTEM_H
#define MOTOR_WINDOW_SYSTEM_H

#include "motor/core/system.h"
#include "motor/core/type_traits.h"
#include <SDL.h>
#include <string>

namespace motor {

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
};

class window_system {
public:
    window_system(entt::registry& reg);
    ~window_system();

    void operator()();

private:
    entt::registry& reg;
    window_config config;
    SDL_Window* window{};
};

} // namespace motor

#endif // MOTOR_WINDOW_SYSTEM_H
