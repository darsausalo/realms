#ifndef MOTOR_MODS_PLUGIN_HPP
#define MOTOR_MODS_PLUGIN_HPP

#include "core/internal_events.hpp"
#include "motor/entity/prototype_registry.hpp"
#include "motor/mods/mod.hpp"
#include "platform/file_watcher.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <memory>
#include <vector>

namespace motor {

class app_builder;
class prototype_registry;

class mods_plugin {
public:
    mods_plugin(app_builder& app);

private:
    entt::registry& registry;
    entt::dispatcher& dispatcher;
    prototype_registry& prototypes;
    file_watcher watcher;

    std::vector<mod> mods{};
    std::vector<mod> broken_mods{};

    void load_prototypes();
    void start_watch_mods();

    void receive_file_changed(const event::file_changed& e);
};

} // namespace motor

#endif // MOTOR_MODS_PLUGIN_HPP
