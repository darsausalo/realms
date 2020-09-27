#include "mods_system.h"
#include "motor/services/locator.h"
#include "motor/services/mods_service.h"
#include "motor/services/scripts_service.h"
#include <spdlog/spdlog.h>

namespace motor {

void mods_system::on_start(entt::registry& reg) {
    locator::mods::set<mods_service>();
    locator::scripts::set<scripts_service>();
    spdlog::info("mods_system::started");
}

void mods_system::on_stop(entt::registry& reg) {
    locator::scripts::reset();
    locator::mods::reset();
    spdlog::info("mods_system::stopped");
}

} // namespace motor
