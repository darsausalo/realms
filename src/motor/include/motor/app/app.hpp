#ifndef MOTOR_APP_HPP
#define MOTOR_APP_HPP

#include "motor/core/events.hpp"
#include "motor/entity/prototype_registry.hpp"
#include "motor/entity/scheduler.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <functional>
#include <memory>
#include <vector>

namespace motor {

class app_builder;

class app {
    friend app_builder;

    struct plugin_data {
        std::string_view name;
        std::unique_ptr<void, void (*)(void*)> instance;
    };

    app();

public:
    entt::registry registry{};
    scheduler scheduler{};
    entt::dispatcher& dispatcher;
    prototype_registry& prototypes;

    app(app&&) = default;
    app& operator=(app&&) = default;

    ~app();

    static app_builder build();

    void run();

private:
    bool should_quit{};
    std::vector<plugin_data> plugins{};

    void receive(const event::quit&) { should_quit = true; }
};

} // namespace motor

#endif // MOTOR_APP_HPP
