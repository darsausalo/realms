#ifndef MOTOR_MODS_SERVICE_H
#define MOTOR_MODS_SERVICE_H

#include "motor/host/mod.h"
#include <memory>
#include <vector>

namespace motor {

class system_dispatcher;

class mods_service {
public:
    mods_service();
    mods_service(const mods_service&) = delete;
    mods_service(mods_service&&) = delete;

    mods_service& operator=(const mods_service&) = delete;
    mods_service& operator=(mods_service&&) = delete;

    ~mods_service();

    void load_plugins();

    void start_plugins(system_dispatcher& dispatcher);
    void stop_plugins(system_dispatcher& dispatcher);

    void reload_plugins(system_dispatcher& dispatcher);

    void load_prefabs(entt::registry& reg);
    void load_snapshot(entt::registry& reg);
    void save_snapshot(entt::registry& reg);

private:
    std::vector<mod> mods{};
    std::vector<mod> broken_mods{};
};

} // namespace motor

#endif // MOTOR_MODS_SERVICE_H