#ifndef MOTOR_STATE_HPP
#define MOTOR_STATE_HPP

#include "motor/app/transition.hpp"
#include "motor/entity/system_dispatcher.hpp"
#include <entt/entity/registry.hpp>

namespace motor {

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

#endif // MOTOR_STATE_HPP
