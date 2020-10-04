#ifndef MOTOR_MODS_SYSTEM_H
#define MOTOR_MODS_SYSTEM_H

#include "motor/core/system.h"
#include "motor/core/type_traits.h"
#include <memory>
#include <vector>

namespace motor {

class mods_system {
public:
    mods_system() noexcept;
    ~mods_system() noexcept;
};

} // namespace motor

#endif // MOTOR_MODS_SYSTEM_H
