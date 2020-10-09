#ifndef MOTOR_MODS_SYSTEM_HPP
#define MOTOR_MODS_SYSTEM_HPP

#include "core/internal_events.hpp"
#include "core/progress.hpp"
#include "motor/entity/prototype_registry.hpp"
#include "motor/mods/mod.hpp"
#include "platform/file_watcher.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <memory>
#include <thread>
#include <vector>

namespace motor {

class app_builder;
class prototype_registry;

class mods_system {
public:
    mods_system(app_builder& app);
    ~mods_system();

private:
    entt::registry& registry;
    entt::dispatcher& dispatcher;
    prototype_registry& prototypes;
    file_watcher watcher;

    bool loaded{};
    std::thread thread{};
    motor::progress prg{};

    std::vector<mod> mods{};
    std::vector<mod> broken_mods{};

    void update();

    void load_prototypes();
    void start_watch_mods();

    void receive_bootstrap(const event::bootstrap&);
    void receive_file_changed(const event::file_changed& e);
};

} // namespace motor

#endif // MOTOR_MODS_SYSTEM_HPP
