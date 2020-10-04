#ifndef MOTOR_STATE_H
#define MOTOR_STATE_H

#include "motor/host/transition.h"
#include <entt/entity/registry.hpp>
#include <motor/core/system_dispatcher.h>

namespace motor {

class system_dispatcher;

class state {
public:
    virtual ~state();

    virtual transition update() { return transition_none{}; }

protected:
    entt::registry& reg;

    state(entt::registry& reg);

    template<system_group Group, typename System, typename... Args>
    void add_system(Args&&... args) {
        auto type_id = reg.ctx<system_dispatcher>().add<Group, System>(
                std::forward<Args>(args)...);
        systems.push_back(type_id);
    }

private:
    std::vector<entt::id_type> systems{};
};

} // namespace motor

#endif // MOTOR_STATE_H
