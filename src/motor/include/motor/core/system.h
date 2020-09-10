#ifndef MOTOR_SYSTEM_H
#define MOTOR_SYSTEM_H

#include <entt/entity/fwd.hpp>

namespace motor {

enum class system_group { init, sim, present };

class system {
public:
    system() noexcept = default;
    virtual ~system() noexcept = default;

    virtual void on_start(entt::registry& reg) {}
    virtual void on_stop(entt::registry& reg) {}
    virtual void update(entt::registry& reg) {}
};

class init_system : public system {
public:
    static constexpr auto group = system_group::init;
};

class sim_system : public system {
public:
    static constexpr auto group = system_group::sim;
};

class present_system : public system {
public:
    static constexpr auto group = system_group::present;
};

} // namespace motor

#endif // MOTOR_SYSTEM_H
