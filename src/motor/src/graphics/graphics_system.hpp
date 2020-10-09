#ifndef MOTOR_GRAPHICS_SYSTEM_HPP
#define MOTOR_GRAPHICS_SYSTEM_HPP

#include "motor/graphics/screen.hpp"

namespace motor {

class app_builder;

class graphics_system {
public:
    graphics_system(app_builder& app);
    ~graphics_system();

private:
    screen& screen;

    void update();
};

} // namespace motor

#endif // MOTOR_GRAPHICS_SYSTEM_HPP
