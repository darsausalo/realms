#include "window_plugin.hpp"
#include "core/internal_events.hpp"
#include "motor/app/app_builder.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <fmt/format.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
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
                       {"vsync", c.vsync},
                       {"position", c.position},
                       {"size", c.size}};
}

void from_json(const nlohmann::json& j, window_config& c) {
    j.at("monitor").get_to(c.monitor);
    j.at("fullscreen").get_to(c.fullscreen);
    j.at("position").get_to(c.position);
    j.at("size").get_to(c.size);
    j.at("vsync").get_to(c.vsync);
}

window_plugin::window_plugin(app_builder& app)
    : dispatcher{app.dispatcher()}
    , screen{app.registry().set<motor::screen>()}
    , jconfig{app.registry().ctx<nlohmann::json>()}
    , config{"default", false, {0, 0}, {1280, 768}} {
    try {
        jconfig.at("window").get_to(config);
    } catch (nlohmann::json::exception& e) {
        spdlog::warn("invalid window config: {}", e.what());
    }

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    int x =
        config.position.x != 0 ? config.position.x : SDL_WINDOWPOS_UNDEFINED;
    int y =
        config.position.y != 0 ? config.position.y : SDL_WINDOWPOS_UNDEFINED;
    window = SDL_CreateWindow("frontier", x, y, config.size.width,
                              config.size.height, SDL_WINDOW_OPENGL);
    if (!window) {
        throw std::runtime_error(
            fmt::format("Could not create window: {}", SDL_GetError()));
    }

    ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, ctx);
    SDL_GL_SetSwapInterval(config.vsync);

    SDL_GL_GetDrawableSize(window, &screen.width, &screen.height);

    app.registry().set<window_context>(window, ctx);

    app.add_system_to_stage<&window_plugin::update_position>(
        "pre_frame"_hs, *this);
    app.add_system_to_stage<&window_plugin::poll_events>("event"_hs, *this);
    app.add_system_to_stage<&window_plugin::swap_buffers>(
        "post_frame"_hs, *this);

    // TODO: poll_events should be sync point
}

window_plugin::~window_plugin() {
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(window);
}

void window_plugin::update_position() {
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
            jconfig["window"] = config;
            dispatcher.enqueue<event::config_changed>();
        } catch (nlohmann::json::exception& e) {
            spdlog::error("failed to update config['window']: {}", e.what());
        }
    }
}

void window_plugin::poll_events() {
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event)) {
        ImGui_ImplSDL2_ProcessEvent(&sdl_event);

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
        case SDL_TEXTINPUT:
            dispatcher.enqueue<event::text_input>({sdl_event.text.text});
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
                {sdl_event.motion.x, sdl_event.motion.y, sdl_event.motion.xrel,
                 sdl_event.motion.yrel});
            break;
        case SDL_MOUSEWHEEL:
            dispatcher.enqueue<event::mouse_wheel_input>(
                {sdl_event.wheel.x, sdl_event.wheel.y});
            break;
        }
    }

    dispatcher.update();
}

void window_plugin::swap_buffers() { SDL_GL_SwapWindow(window); }

} // namespace motor
