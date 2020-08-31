#ifndef MOTOR_WINDOW_SYSTEM_H
#define MOTOR_WINDOW_SYSTEM_H

#include "motor/systems/system.h"
#include <SDL.h>
#include <nlohmann/json_fwd.hpp>
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

class window_system : public system {
public:
    window_system(const nlohmann::json& jconfig);

    void on_start(game_data& data) override;
    void on_stop(game_data& data) override;
    void update(game_data& data) override;

private:
    SDL_Window* window{};
    window_config config{};
};

} // namespace motor

#endif // MOTOR_WINDOW_SYSTEM_H
