#include "event_system.hpp"
#include "motor/app/app.hpp"
#include <SDL.h>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace motor {

event_system::event_system(entt::registry& registry) noexcept
    : registry{registry}, dispatcher{registry.ctx<entt::dispatcher>()} {
    spdlog::debug("event_system::start");
}

event_system::~event_system() {
    spdlog::debug("event_system::stop");
}

void event_system::operator()() {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        if (sdl_event.type == SDL_QUIT) {
            dispatcher.enqueue<event::quit>();
        }
    }

    dispatcher.update();
}

} // namespace motor
