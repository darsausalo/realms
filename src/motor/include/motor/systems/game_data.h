#ifndef MOTOR_GAME_DATA_H
#define MOTOR_GAME_DATA_H

#include <entt/signal/dispatcher.hpp>

namespace motor {

class game_data {
public:
    entt::dispatcher event_dispatcher{};

public:
    game_data() noexcept = default;
    ~game_data() noexcept = default;
};

} // namespace motor

#endif // MOTOR_GAME_DATA_H
