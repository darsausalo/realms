#include "mods_system.h"
#include "motor/services/locator.h"
#include "motor/services/mods_service.h"
#include "motor/services/scripts_service.h"
#include <spdlog/spdlog.h>

namespace motor {

// TODO: use file watcher in mods system

mods_system::mods_system() noexcept {
    locator::mods::set<mods_service>();
    locator::scripts::set<scripts_service>();
    spdlog::debug("mods_system::start");
}

mods_system::~mods_system() noexcept {
    spdlog::debug("mods_system::stop");
    locator::scripts::reset();
    locator::components::reset();
    locator::mods::reset();
}

} // namespace motor
