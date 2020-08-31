#ifndef MOTOR_SYSTEM_H
#define MOTOR_SYSTEM_H

#include <entt/core/fwd.hpp>

namespace motor {

class context;

class system {
public:
    system() noexcept = default;
    virtual ~system() noexcept = default;

    virtual void on_start(context& data) {}
    virtual void on_stop(context& data) {}
    virtual void update(context& data) {}
};

} // namespace motor

#endif // MOTOR_SYSTEM_H
