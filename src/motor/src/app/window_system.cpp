#include "window_system.hpp"
#include "config_system.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
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

window_system::window_system(entt::registry& reg)
    : reg{reg}, config{"default", false, {0, 0}, {1280, 768}} {
    try {
        reg.ctx<core_config>().at("window").get_to(config);
    } catch (nlohmann::json::exception& e) {
        spdlog::warn("invalid window config: {}", e.what());
    }

    int x = config.position.x != 0 ? config.position.x
                                   : SDL_WINDOWPOS_UNDEFINED;
    int y = config.position.y != 0 ? config.position.y
                                   : SDL_WINDOWPOS_UNDEFINED;
    window = SDL_CreateWindow("frontier", x, y, config.size.width,
                              config.size.height, SDL_WINDOW_OPENGL);
    if (!window) {
        throw std::runtime_error(
                fmt::format("Could not create window: {}", SDL_GetError()));
    }

    spdlog::debug("window_system::start");
}

window_system::~window_system() {
    spdlog::debug("window_system::stop");
    SDL_DestroyWindow(window);
}

void window_system::operator()() {
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
            reg.ctx<core_config>()["window"] = config;
            reg.ctx<entt::dispatcher>().trigger<event::config_changed>();
        } catch (nlohmann::json::exception& e) {
            spdlog::error("failed to update config['window']: {}", e.what());
        }
    }
}

} // namespace motor
