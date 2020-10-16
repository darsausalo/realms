#ifndef MOTOR_APP_HPP
#define MOTOR_APP_HPP

#include "motor/core/events.hpp"
#include "motor/entity/prototype_registry.hpp"
#include "motor/entity/scheduler.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <functional>
#include <memory>
#include <thread>
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
    app(app&&) = default;
    app& operator=(app&&) = default;

    ~app();

    static app_builder& build();

    void run();

private:
    entt::registry registry{};
    scheduler scheduler{};
    entt::dispatcher& dispatcher;
    prototype_registry& prototypes;
    bool should_quit{};
    std::vector<plugin_data> plugins{};
    std::thread startup_thread{};

    void request_quit() { should_quit = true; }

    void startup(entt::registry& startup_registry);
};

} // namespace motor

#endif // MOTOR_APP_HPP
