#ifndef MOTOR_EVENT_SYSTEM_HPP
#define MOTOR_EVENT_SYSTEM_HPP

#include <entt/entity/fwd.hpp>

namespace motor {

class event_system {
public:
    event_system(entt::registry& reg) noexcept;
    ~event_system() noexcept;

    void operator()();

private:
    entt::registry& reg;
};

} // namespace motor

#endif // MOTOR_EVENT_SYSTEM_HPP
