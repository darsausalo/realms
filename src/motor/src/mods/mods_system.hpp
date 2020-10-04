#ifndef MOTOR_MODS_SYSTEM_HPP
#define MOTOR_MODS_SYSTEM_HPP

#include "core/progress.hpp"
#include "motor/core/event.hpp"
#include <entt/entity/fwd.hpp>
#include <memory>
#include <thread>
#include <vector>

namespace motor {

class mods_system {
public:
    mods_system(entt::registry& reg);
    ~mods_system();

    void operator()();

private:
    entt::registry& reg;
    bool loaded{};
    std::thread thread{};
    motor::progress prg{};
};

} // namespace motor

#endif // MOTOR_MODS_SYSTEM_HPP
