#ifndef MOTOR_SCRIPTS_SERVICE_H
#define MOTOR_SCRIPTS_SERVICE_H

#include <sol/sol.hpp>

namespace motor {

class prototype_registry;

class scripts_service {
public:
    scripts_service() = default;
    scripts_service(const scripts_service&) = delete;
    scripts_service(scripts_service&&) = delete;

    scripts_service& operator=(const scripts_service&) = delete;
    scripts_service& operator=(scripts_service&&) = delete;

    ~scripts_service() = default;

    void run_scripts();

    sol::table load_prototypes(sol::state& prototypes_lua);

private:
    sol::state lua{};
};

} // namespace motor

#endif // MOTOR_SCRIPTS_SERVICE_H
