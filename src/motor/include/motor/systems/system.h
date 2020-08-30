#ifndef MOTOR_SYSTEM_H
#define MOTOR_SYSTEM_H

#include <entt/core/fwd.hpp>

namespace motor {

class game_data;

class system {
public:
    system() noexcept = default;
    virtual ~system() noexcept = default;

    virtual void on_start(game_data& data) {}
    virtual void on_stop(game_data& data) {}
    virtual void update(game_data& data) {}
};

} // namespace motor

#endif // MOTOR_SYSTEM_H
