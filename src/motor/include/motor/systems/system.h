#ifndef MOTOR_SYSTEM_H
#define MOTOR_SYSTEM_H

#include <entt/fwd.hpp>

namespace motor {

class system {
public:
    system() noexcept = default;
    virtual ~system() noexcept = default;

    virtual void on_start(entt::registry& reg) {}
    virtual void on_stop(entt::registry& reg) {}
    virtual void update(entt::registry& reg) {}
};

} // namespace motor

#endif // MOTOR_SYSTEM_H
