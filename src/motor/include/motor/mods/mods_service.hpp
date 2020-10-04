#ifndef MOTOR_MODS_SERVICE_H
#define MOTOR_MODS_SERVICE_H

#include "motor/core/prototype_registry.h"
#include "motor/mods/mod.h"
#include <memory>
#include <sol/sol.hpp>
#include <vector>

namespace motor {

class mods_service {
public:
    mods_service();
    mods_service(const mods_service&) = delete;
    mods_service(mods_service&&) = delete;

    mods_service& operator=(const mods_service&) = delete;
    mods_service& operator=(mods_service&&) = delete;

    ~mods_service();

    template<typename Func>
    void visit(Func func) const {
        for (const auto& m : mods) {
            func(m);
        }
    }

private:
    friend class mods_system;

    std::vector<mod> mods{};
    std::vector<mod> broken_mods{};

    void load_prototypes(prototype_registry& prototypes);
    sol::table load_prototypes(sol::state& prototypes_lua);

    void run_mod_scripts(sol::state& lua, std::string_view name,
                         const std::vector<std::string_view>& libs,
                         const std::vector<std::string_view>& preloads);
};

} // namespace motor

#endif // MOTOR_MODS_SERVICE_H
