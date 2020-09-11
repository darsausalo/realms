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

class window_system : public init_system {
public:
    window_system() noexcept;

    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    void update(entt::registry& reg) override;

private:
    SDL_Window* window{};
    window_config config{};
};

template<>
struct is_host_type<window_system> : std::true_type {};

} // namespace motor

#endif // MOTOR_WINDOW_SYSTEM_H
