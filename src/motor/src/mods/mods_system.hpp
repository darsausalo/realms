#ifndef MOTOR_MODS_SYSTEM_HPP
#define MOTOR_MODS_SYSTEM_HPP

#include "core/progress.hpp"
#include "motor/core/event.hpp"
#include "platform/file_watcher.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <memory>
#include <thread>
#include <vector>

namespace motor {

class prototype_registry;

class mods_system {
public:
    mods_system(entt::registry& registry);
    ~mods_system();

    void operator()();

private:
    entt::registry& registry;
    entt::dispatcher& dispatcher;
    prototype_registry& prototype_registry;
    file_watcher watcher;

    bool loaded{};
    std::thread thread{};
    motor::progress prg{};

    void start_watch_mods();
    void receive_file_changed(const event::file_changed& e);
};

} // namespace motor

#endif // MOTOR_MODS_SYSTEM_HPP