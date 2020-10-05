#ifndef MOTOR_EVENT_SYSTEM_HPP
#define MOTOR_EVENT_SYSTEM_HPP

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace motor {

class event_system {
public:
    event_system(entt::registry& registry) noexcept;
    ~event_system() noexcept;

    void operator()();

private:
    entt::registry& registry;
    entt::dispatcher& dispatcher;
};

} // namespace motor

#endif // MOTOR_EVENT_SYSTEM_HPP
