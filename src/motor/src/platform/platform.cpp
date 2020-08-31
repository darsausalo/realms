#include "motor/platform/platform.h"
#include "backward.hpp"
#include "motor/core/exception.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace motor {

platform::platform() {
    setup_crash_handling(SDL_GetBasePath());

    base_path = SDL_GetBasePath();
    data_path = base_path / MOTOR_DATA_DIR;
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

    if (!std::filesystem::exists(user_path)) {
        try {
            std::filesystem::create_directories(user_path);
        } catch (std::filesystem::filesystem_error& e) {
            spdlog::error("failed to create user dir: {}", e.what());
        }
    }
}

std::filesystem::path
platform::get_full_path(const std::filesystem::path& relative_path,
                        bool for_write) const {
    if (for_write) {
        if (!std::filesystem::exists(user_path)) {
            return data_path / relative_path;
        }
        return user_path / relative_path;
    }
    auto path = user_path / relative_path;
    if (std::filesystem::exists(path)) {
        return path;
    }
    return data_path / relative_path;
}

} // namespace motor

namespace backward {

SignalHandling sh;

std::string report_path;

} // namespace backward
