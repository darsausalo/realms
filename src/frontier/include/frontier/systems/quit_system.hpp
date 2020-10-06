#ifndef FRONTIER_QUIT_SYSTEM_HPP
#define FRONTIER_QUIT_SYSTEM_HPP

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <motor/core/events.hpp>
#include <motor/core/input.hpp>
#include <spdlog/spdlog.h>

namespace frontier {

class quit_system {
public:
    quit_system(entt::registry& registry)
        : actions{registry.ctx<motor::input_actions>()},
          dispatcher{registry.ctx<entt::dispatcher>()} {}

    void operator()() {
        if (actions.is_just_pressed("quit"_hs)) {
            dispatcher.enqueue<motor::event::quit>();
        }
    }

private:
    motor::input_actions& actions;
    entt::dispatcher& dispatcher;
};

} // namespace frontier

#endif // FRONTIER_QUIT_SYSTEM_HPP
