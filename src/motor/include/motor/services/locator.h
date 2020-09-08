#ifndef MOTOR_SERVICE_LOCATOR_H
#define MOTOR_SERVICE_LOCATOR_H

#include <entt/locator/locator.hpp>

namespace motor {

class files_service;

struct locator {
    using files = entt::service_locator<files_service>;
};

} // namespace motor

#endif // MOTOR_SERVICE_LOCATOR_H
