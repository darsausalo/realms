#ifndef MOTOR_APP_HPP
#define MOTOR_APP_HPP

#include "motor/core/events.hpp"
#include "motor/entity/system_dispatcher.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

namespace motor {

class state;

class app {
    using create_state_fn = std::function<std::shared_ptr<state>()>;

public:
    app(const app&) = delete;
    app(app&&) = delete;

    app& operator=(const app&) = delete;
    app& operator=(app&&) = delete;

    const entt::registry& get_registry() const { return registry; }
    entt::registry& get_registry() { return registry; }

    template<typename System, stage Stage = stage::ON_UPDATE, typename... Args>
    entt::id_type add_system(Args&&... args) {
        return dispatcher.add<System, Stage>(std::forward<Args>(args)...);
    }

    template<stage Stage = stage::ON_UPDATE, typename Func>
    auto add_system(Func func) {
        return dispatcher.add<Stage, Func>(func);
    }

    void remove_system(entt::id_type type_id) { dispatcher.remove(type_id); }

    template<typename InitialState>
    int run() {
        run_loop(std::move(std::make_shared<InitialState>(*this)));
        return 0;
    }

protected:
    entt::registry registry{};
    system_dispatcher dispatcher{};

    app(int argc, const char* argv[]);

private:
    bool quit_requested{};

    bool should_close();

    void run_loop(std::shared_ptr<state>&& initial_state);

    void receive_quit(const event::quit&) { quit_requested = true; }
};

} // namespace motor

#endif // MOTOR_APP_HPP
