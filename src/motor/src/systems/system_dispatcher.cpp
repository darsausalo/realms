#include "motor/systems/system_dispatcher.h"

namespace motor {

system_dispatcher::~system_dispatcher() {
    while (!systems.empty()) {
        systems.back()->on_stop(ctx);
        systems.pop_back();
    }
}

void system_dispatcher::update() {
    for (auto& system : systems) {
        system->update(ctx);
    }
}

void system_dispatcher::sort() {
    // TODO: sort by dependency
}

} // namespace motor
