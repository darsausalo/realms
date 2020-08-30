#ifndef MOTOR_WINDOW_SYSTEM_H
#define MOTOR_WINDOW_SYSTEM_H

#include "motor/systems/system.h"
#include <SDL.h>

namespace motor {

class window_system : public system {
public:
    void on_start(game_data& data) override;
    void on_stop(game_data& data) override;
    void update(game_data& data) override;

private:
    SDL_Window* window{};
};

} // namespace motor

#endif // MOTOR_WINDOW_SYSTEM_H
