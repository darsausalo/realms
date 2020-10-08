#ifndef MOTOR_GRAPHICS_SYSTEM_HPP
#define MOTOR_GRAPHICS_SYSTEM_HPP

#include "motor/graphics/screen.hpp"
#include <entt/entity/fwd.hpp>

namespace motor {

class graphics_system {
public:
    graphics_system(entt::registry& registry);
    ~graphics_system();

    void operator()();

private:
    entt::registry& registry;
    screen& screen;
};

} // namespace motor

#endif // MOTOR_GRAPHICS_SYSTEM_HPP
