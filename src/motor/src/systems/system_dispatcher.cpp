#include "motor/systems/system_dispatcher.h"

namespace motor {

void system_dispatcher::start(game_data& data) {
    for (auto& system : systems) {
        system->on_start(data);
    }
}

void system_dispatcher::stop(game_data& data) {
    for (auto& system : systems) {
        system->on_stop(data);
    }
}

void system_dispatcher::update(game_data& data) {
    for (auto& system : systems) {
        system->update(data);
    }
}


} // namespace motor
