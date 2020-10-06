#include "event_system.hpp"
#include "motor/app/app.hpp"
#include "motor/core/event.hpp"
#include "motor/core/input.hpp"
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
        switch (sdl_event.type) {
        case SDL_QUIT:
            dispatcher.enqueue<event::quit>();
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            dispatcher.enqueue<event::keyboard_input>(
                    {static_cast<std::uint16_t>(sdl_event.key.keysym.scancode),
                     static_cast<std::uint16_t>(sdl_event.key.keysym.sym),
                     sdl_event.type == SDL_KEYDOWN && !sdl_event.key.repeat,
                     sdl_event.type == SDL_KEYUP,
                     static_cast<bool>(sdl_event.key.repeat)});
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (sdl_event.button.button == SDL_BUTTON_LEFT ||
                sdl_event.button.button == SDL_BUTTON_RIGHT)
                dispatcher.enqueue<event::mouse_button_input>(
                        {sdl_event.button.button == SDL_BUTTON_LEFT ? 0u : 1u,
                         sdl_event.button.clicks,
                         static_cast<bool>(sdl_event.button.state)});
            break;
        case SDL_MOUSEMOTION:
            dispatcher.enqueue<event::mouse_motion_input>(
                    {sdl_event.motion.x, sdl_event.motion.y,
                     sdl_event.motion.xrel, sdl_event.motion.yrel});
            break;
        case SDL_MOUSEWHEEL:
            dispatcher.enqueue<event::mouse_wheel_input>(
                    {sdl_event.wheel.x, sdl_event.wheel.y});
            break;
        }
    }

    dispatcher.update();
}

} // namespace motor
