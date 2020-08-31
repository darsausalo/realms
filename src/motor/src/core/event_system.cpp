#include "event_system.h"
#include "motor/core/events.h"
#include "motor/systems/context.h"
#include <SDL.h>
#include <spdlog/spdlog.h>

namespace motor {

void event_system::on_start(context& ctx) {
    spdlog::debug("event_system::started");
}

void event_system::on_stop(context& ctx) {
    spdlog::debug("event_system::stopped");
}

void event_system::update(context& ctx) {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        if (sdl_event.type == SDL_QUIT) {
            ctx.get<entt::dispatcher>().trigger<event::quit>();
        }
    }
}

} // namespace motor
