#include "platform.h"
#include "backward.hpp"
#include "motor/core/exception.h"
#include <fmt/core.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace motor::platform {

void setup_crash_handling(std::string_view base_path);

void initialize() {
    SDL_SetMainReady();
    setup_crash_handling(SDL_GetBasePath());
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        throw exception(
                fmt::format("SDL initialization failed: {}", SDL_GetError()));
    }
}

void shutdown() {
    SDL_Quit();
}

} // namespace motor::platform

namespace backward {

SignalHandling sh;

std::string report_path;

} // namespace backward
