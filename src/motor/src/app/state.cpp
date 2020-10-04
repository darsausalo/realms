#include "motor/app/state.hpp"
#include <entt/entity/registry.hpp>

namespace motor {

state::state(entt::registry& reg) : reg{reg} {
}

state::~state() {
    while (!systems.empty()) {
        reg.ctx<system_dispatcher>().remove(systems.back());
        systems.pop_back();
    }
}

} // namespace motor
