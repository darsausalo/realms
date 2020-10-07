#ifndef MOTOR_SERVICE_LOCATOR_HPP
#define MOTOR_SERVICE_LOCATOR_HPP

#include <entt/locator/locator.hpp>

namespace motor {

class files_service;

struct locator {
    using files = entt::service_locator<files_service>;
};

} // namespace motor

#endif // MOTOR_SERVICE_LOCATOR_HPP
