#ifndef MOTOR_SYSTEM_H
#define MOTOR_SYSTEM_H

#include <entt/entity/fwd.hpp>

namespace motor {

enum class system_group {
    pre_frame,
    on_load,
    post_load,
    pre_update,
    on_update,
    on_validate,
    post_update,
    pre_store,
    on_store,
    post_frame
};

class system_base {
public:
    system_base() noexcept = default;
    virtual ~system_base() noexcept = default;

    virtual void on_start(entt::registry& reg) {}
    virtual void on_stop(entt::registry& reg) {}
    virtual void update(entt::registry& reg) {}
};

template<system_group SystemGroup>
class system : public system_base {
public:
    static constexpr auto group = SystemGroup;
};

} // namespace motor

#endif // MOTOR_SYSTEM_H
