#include "motor/platform/platform.h"
#include "backward.hpp"
#include "motor/core/exception.h"
#include <fmt/format.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace motor {

platform::platform() {
    setup_crash_handling(SDL_GetBasePath());

    base_path = SDL_GetBasePath();
    data_path = std::filesystem::path(SDL_GetBasePath()) / MOTOR_DATA_DIR;
    user_path = SDL_GetPrefPath(MOTOR_PROJECT_ORG, MOTOR_PROJECT_NAME);

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        throw exception(
                fmt::format("SDL initialization failed: {}", SDL_GetError()));
    }
}

platform::~platform() {
    SDL_Quit();
}

void platform::set_data_path(const std::filesystem::path& path) {
    data_path = path;
}

void platform::set_user_path(const std::filesystem::path& path) {
    user_path = path;
}

} // namespace motor

namespace backward {

SignalHandling sh;

std::string report_path;

} // namespace backward
