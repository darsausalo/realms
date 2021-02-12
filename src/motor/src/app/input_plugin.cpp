#include "app/input_plugin.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/core/algorithm.hpp"
#include <SDL.h>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string_view>

namespace motor {

static constexpr auto MBL = "MBL"_hs;
static constexpr auto MBR = "MBR"_hs;
static constexpr auto MWU = "MWU"_hs;
static constexpr auto MWD = "MWD"_hs;

static constexpr auto MOD_NAME_CTRL = "CTRL";
static constexpr auto MOD_NAME_SHIFT = "SHIFT";
static constexpr auto MOD_NAME_ALT = "ALT";

static std::uint16_t get_key_modifiers() {
    std::uint16_t key_modifiers = 0u;
    auto modifiers_state = SDL_GetModState();
    if (modifiers_state & KMOD_CTRL) {
        key_modifiers |= KMOD_CTRL;
    }
    if (modifiers_state & KMOD_SHIFT) {
        key_modifiers |= KMOD_SHIFT;
    }
    if (modifiers_state & KMOD_ALT) {
        key_modifiers |= KMOD_ALT;
    }
    return key_modifiers;
}

static action_key parse_action_key(std::string_view text) {
    auto parts = split(text, "+");
    std::uint16_t key_modifiers = 0u;
    std::string key_name{};
    for (auto&& s : parts) {
        std::string name{s};
        std::transform(name.begin(), name.end(), name.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        if (name == MOD_NAME_CTRL) {
            key_modifiers |= KMOD_CTRL;
        } else if (name == MOD_NAME_SHIFT) {
            key_modifiers |= KMOD_SHIFT;
        } else if (name == MOD_NAME_ALT) {
            key_modifiers |= KMOD_ALT;
        } else {
            key_name = name;
        }
    }
    // TODO: validate key_name
    if (key_name.empty()) {
        spdlog::error("invalid action key: {}", text);
    }
    return {entt::hashed_string{std::data(key_name)}, key_modifiers};
}

static void parse_keymap(
    nlohmann::json config,
    std::unordered_map<action_key, entt::id_type>& keymap) {
    for (auto&& [key, value] : config.items()) {
        auto action_key = parse_action_key(key);
        auto action_name = value.at("action").get<std::string>();
        keymap.insert_or_assign(
            action_key, entt::hashed_string{std::data(action_name)});
    }
}

input_plugin::input_plugin(app_builder& app)
    : dispatcher{app.dispatcher()}
    , jconfig{app.registry().ctx_or_set<nlohmann::json>()}
    , pointer_position{app.registry().set<cursor_position>()}
    , actions{app.registry().set<input_actions>()} {
    dispatcher.sink<event::keyboard_input>()
        .connect<&input_plugin::receive_keyboard_input>(*this);
    dispatcher.sink<event::mouse_button_input>()
        .connect<&input_plugin::receive_mouse_button_input>(*this);
    dispatcher.sink<event::mouse_motion_input>()
        .connect<&input_plugin::receive_mouse_motion_input>(*this);
    dispatcher.sink<event::mouse_wheel_input>()
        .connect<&input_plugin::receive_mouse_wheel_input>(*this);

    SDL_GetMouseState(&pointer_position.x, &pointer_position.y);

    try {
        if (jconfig.contains("input") &&
            jconfig.at("input").contains("keymap")) {
            parse_keymap(jconfig.at("input").at("keymap"), keymap);
        }
    } catch (nlohmann::json::exception& e) {
        spdlog::warn("invalid input config: {}", e.what());
    }

    app.add_system_to_stage<&input_plugin::update_actions>(
        "pre_event"_hs, *this);
}

input_plugin::~input_plugin() { dispatcher.disconnect(*this); }

void input_plugin::update_actions() { actions.update(); }

bool input_plugin::handle_ui_keyboard(const event::keyboard_input& e) {
    // TODO: use ImGui
    return false;
}

bool input_plugin::handle_ui_mouse_button(const event::mouse_button_input& e) {
    // TODO: use ImGui
    return false;
}

bool input_plugin::handle_ui_mouse_motion(const event::mouse_motion_input& e) {
    // TODO: use ImGui
    return false;
}

bool input_plugin::handle_ui_mouse_wheel(const event::mouse_wheel_input& e) {
    // TODO: use ImGui
    return false;
}

void input_plugin::receive_keyboard_input(const event::keyboard_input& e) {
    if (!handle_ui_keyboard(e) && (e.pressed || e.released)) {
        entt::hashed_string key_name{
            SDL_GetKeyName(static_cast<SDL_Keycode>(e.key_code))};
        action_key key{key_name, get_key_modifiers()};
        if (auto it = keymap.find(key); it != keymap.cend()) {
            if (e.pressed) {
                actions.press(it->second);
            } else {
                actions.release(it->second);
            }
        }
    }
}

void input_plugin::receive_mouse_button_input(
    const event::mouse_button_input& e) {
    if (!handle_ui_mouse_button(e)) {
        auto key_name = e.key == 0 ? MBL : MBR;
        action_key key{key_name, get_key_modifiers()};
        if (auto it = keymap.find(key); it != keymap.cend()) {
            if (e.pressed) {
                actions.press(it->second);
            } else {
                actions.release(it->second);
            }
        }
    }
}

void input_plugin::receive_mouse_motion_input(
    const event::mouse_motion_input& e) {
    if (!handle_ui_mouse_motion(e)) {
        pointer_position.x = e.x;
        pointer_position.y = e.y;
    }
}

void input_plugin::receive_mouse_wheel_input(
    const event::mouse_wheel_input& e) {
    if (!handle_ui_mouse_wheel(e) && e.y != 0) {
        auto key_name = e.y > 0 ? MWU : MWD;
        // TODO: mouse wheel without focus not receive key modifiers
        action_key key{key_name, get_key_modifiers()};
        if (auto it = keymap.find(key); it != keymap.cend()) {
            actions.release(it->second);
        }
    }
}

} // namespace motor

//==============================================================================
// TEST

#include <doctest/doctest.h>

namespace {

static const char* json_text = R"({
    "t": { "action": "action1" },
    "ctrl+shift+t": { "action": "action2" },
    "Q": { "action": "action3" },
    "ctrl+q": { "action": "action4" },
    "mbl": { "action": "action5" },
    "mbr": { "action": "action6" },
    "mwu": { "action": "action7" },
    "MWd": { "action": "action8" }
})";

}

TEST_CASE("input_plugin: config") {
    using namespace entt::literals;

    motor::parse_action_key("ctrl+shift+t");

    nlohmann::json j = nlohmann::json::parse(json_text);

    std::unordered_map<motor::action_key, entt::id_type> keymap;
    parse_keymap(j, keymap);

    CHECK(keymap[{"T"_hs, 0}] == "action1"_hs);
    CHECK(keymap[{"T"_hs, KMOD_CTRL | KMOD_SHIFT}] == "action2"_hs);
    CHECK(keymap[{"Q"_hs, 0}] == "action3"_hs);
    CHECK(keymap[{"Q"_hs, KMOD_CTRL}] == "action4"_hs);
    CHECK(keymap[{motor::MBL, 0}] == "action5"_hs);
    CHECK(keymap[{motor::MBR, 0}] == "action6"_hs);
    CHECK(keymap[{motor::MWU, 0}] == "action7"_hs);
    CHECK(keymap[{motor::MWD, 0}] == "action8"_hs);
}
