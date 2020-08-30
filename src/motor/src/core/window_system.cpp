#include "window_system.h"
#include "motor/core/exception.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace motor {

void window_system::on_start(game_data& data) {
    window = SDL_CreateWindow("frontier", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, 640, 480,
                              SDL_WINDOW_OPENGL);
    if (!window) {
        throw exception(
                fmt::format("Could not create window: {}", SDL_GetError()));
    }

    spdlog::debug("window_system::start");
}

void window_system::on_stop(game_data& data) {
    SDL_DestroyWindow(window);
    spdlog::debug("window_system::stop");
}

void window_system::update(game_data& data) {
}

} // namespace motor
