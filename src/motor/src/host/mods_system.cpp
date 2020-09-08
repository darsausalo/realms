#include "mods_system.h"
#include "motor/services/locator.h"
#include "motor/services/mods_service.h"
#include <spdlog/spdlog.h>

namespace motor {

typedef void (*plugin_entry_func)(plugin_context* ctx);

void mods_system::on_start(entt::registry& reg) {
    locator::mods::set<mods_service>();
    spdlog::info("mods_system::started");
}

void mods_system::on_stop(entt::registry& reg) {
    locator::mods::reset();
    spdlog::info("mods_system::stopped");
}

} // namespace motor
