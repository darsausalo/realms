#include "motor/systems/system_dispatcher.h"
#include <algorithm>

namespace motor {

system_dispatcher::~system_dispatcher() {
    while (!systems.empty()) {
        systems.back().instance->on_stop(reg);
        systems.pop_back();
    }
}

void system_dispatcher::update() {
    for (auto& system : systems) {
        system.instance->update(reg);
    }
}

void system_dispatcher::sort() {
    // TODO: topological sorting
    std::sort(systems.begin(), systems.end(), [](auto& a, auto& b) {
        auto ds = std::find_if(b.dependencies.cbegin(), b.dependencies.cend(),
                               [&a](auto&& b_type_id) {
                                   return a.type_id == b_type_id;
                               }) != b.dependencies.end();
        return ds;
    });
}

} // namespace motor
