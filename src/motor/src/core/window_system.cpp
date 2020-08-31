#include "window_system.h"
#include "config_system.h"
#include "motor/core/exception.h"
#include "motor/systems/game_data.h"
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace motor {

void to_json(nlohmann::json& j, const window_size& s) {
    j = nlohmann::json{{"width", s.width}, {"height", s.height}};
}

void from_json(const nlohmann::json& j, window_size& s) {
    j.at("width").get_to(s.width);
    j.at("height").get_to(s.height);
}

void to_json(nlohmann::json& j, const window_position& p) {
    j = nlohmann::json{{"x", p.x}, {"y", p.y}};
}

void from_json(const nlohmann::json& j, window_position& p) {
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
}

void to_json(nlohmann::json& j, const window_config& c) {
    j = nlohmann::json{{"monitor", c.monitor},
                       {"fullscreen", c.fullscreen},
                       {"position", c.position},
                       {"size", c.size}};
}

void from_json(const nlohmann::json& j, window_config& c) {
    j.at("monitor").get_to(c.monitor);
    j.at("fullscreen").get_to(c.fullscreen);
    j.at("position").get_to(c.position);
    j.at("size").get_to(c.size);
}

window_system::window_system(const nlohmann::json& jconfig)
    : config{"default", false, {-1, -1}, {1280, 768}} {
    try {
        jconfig.at("window").get_to(config);
    } catch (nlohmann::json::exception& e) {
        spdlog::warn("invalid window config: {}", e.what());
    }
}

void window_system::on_start(game_data& data) {
    try {

    } catch (nlohmann::json::exception& e) {
        spdlog::warn("config.window: {}", e.what());
    }

    int x = config.position.x != 0 ? config.position.x
                                   : SDL_WINDOWPOS_UNDEFINED;
    int y = config.position.y != 0 ? config.position.y
                                   : SDL_WINDOWPOS_UNDEFINED;
    window = SDL_CreateWindow("frontier", x, y, config.size.width,
                              config.size.height, SDL_WINDOW_OPENGL);
    if (!window) {
        throw exception(
                fmt::format("Could not create window: {}", SDL_GetError()));
    }

    spdlog::debug("window_system::started");
}

void window_system::on_stop(game_data& data) {
    SDL_DestroyWindow(window);
    spdlog::debug("window_system::stopped");
}

void window_system::update(game_data& data) {
    bool modified = false;

    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    if (x != config.position.x || y != config.position.y) {
        config.position.x = x;
        config.position.y = y;
        modified = true;
    }

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    if (w != config.size.width || h != config.size.height) {
        config.size.width = w;
        config.size.height = h;
        modified = true;
    }

    if (modified) {
        try {
            nlohmann::json j = config;
            data.event_dispatcher.trigger(config_changed{"window", j});
        } catch (nlohmann::json::exception& e) {
            spdlog::error("failed to update config['window']: {}", e.what());
        }
    }
}

} // namespace motor
