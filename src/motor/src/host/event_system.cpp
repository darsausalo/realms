#include "event_system.h"
#include "motor/host/events.h"
#include <SDL.h>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace motor {

void event_system::on_start(entt::registry& reg) {
    spdlog::debug("event_system::started");
}

void event_system::on_stop(entt::registry& reg) {
    spdlog::debug("event_system::stopped");
}

void event_system::update(entt::registry& reg) {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        if (sdl_event.type == SDL_QUIT) {
            reg.ctx<entt::dispatcher>().trigger<event::quit>();
        }
    }
}

} // namespace motor
