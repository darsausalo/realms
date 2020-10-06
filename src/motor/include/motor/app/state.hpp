#ifndef MOTOR_STATE_HPP
#define MOTOR_STATE_HPP

#include "motor/app/app.hpp"
#include "motor/app/transition.hpp"
#include <entt/entity/registry.hpp>

namespace motor {

class state {
public:
    virtual ~state();

    virtual transition update() { return transition_none{}; }

protected:
    friend class state_machine;

    app& app;

    state(motor::app& app) : app{app} {}

    virtual void on_start() {}
    virtual void on_stop() {}

    template<typename System, stage Stage = stage::ON_UPDATE, typename... Args>
    void add_system(Args&&... args) {
        auto type_id =
                app.add_system<System, Stage>(std::forward<Args>(args)...);
        systems.push_back(type_id);
    }

    template<stage Stage = stage::ON_UPDATE, typename Func>
    auto add_system(Func func) {
        return dispatcher.add<Stage, Func>(func);
    }

private:
    std::vector<entt::id_type> systems{};
};

} // namespace motor

#endif // MOTOR_STATE_HPP
