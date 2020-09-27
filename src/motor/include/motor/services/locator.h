#ifndef MOTOR_SERVICE_LOCATOR_H
#define MOTOR_SERVICE_LOCATOR_H

#include <entt/locator/locator.hpp>

namespace motor {

class files_service;
class mods_service;
class scripts_service;

struct locator {
    using files = entt::service_locator<files_service>;
    using mods = entt::service_locator<mods_service>;
    using scripts = entt::service_locator<scripts_service>;
};

} // namespace motor

#endif // MOTOR_SERVICE_LOCATOR_H
