#include "motor/app/state.hpp"
#include <entt/entity/registry.hpp>

namespace motor {

state::~state() {
    while (!systems.empty()) {
        app.remove_system(systems.back());
        systems.pop_back();
    }
}

} // namespace motor
