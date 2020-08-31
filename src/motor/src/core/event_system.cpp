#include "event_system.h"
#include "motor/core/events.h"
#include "motor/systems/game_data.h"
#include <SDL.h>
#include <spdlog/spdlog.h>

namespace motor {

void event_system::on_start(game_data& data) {
    spdlog::debug("event_system::started");
}

void event_system::on_stop(game_data& data) {
    spdlog::debug("event_system::stopped");
}

void event_system::update(game_data& data) {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        if (sdl_event.type == SDL_QUIT) {
            data.event_dispatcher.trigger<event::quit>();
        }
    }
}

} // namespace motor
