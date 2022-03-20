#include "app/input_plugin.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/core/algorithm.hpp"
#include <SDL.h>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <imgui.h>
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

static void parse_axismap(nlohmann::json config,
                          std::vector<axis_binding>& axismap) {
    for (auto&& [key, value] : config.items()) {
        std::string axis_key = key;
        auto axis_name = value.at("axis").get<std::string>();

        if (axis_key.size() == 4) {
            axis_binding binding{
                entt::hashed_string::value(
                    std::data(axis_name), axis_name.size()),
                static_cast<uint16_t>(axis_key[0]),
                static_cast<uint16_t>(axis_key[1]),
                static_cast<uint16_t>(axis_key[2]),
                static_cast<uint16_t>(axis_key[3]),
            };
            axismap.push_back(binding);
        } else {
            // TODO: parse gamepad axis
            spdlog::error("invalid axis key: {}", axis_key);
        }
    }
}

input_plugin::input_plugin(app_builder& app)
    : dispatcher{app.dispatcher()}
    , jconfig{app.registry().ctx_or_set<nlohmann::json>()}
    , pointer_position{app.registry().set<cursor_position>()}
    , actions{app.registry().set<input_actions>()}
    , axises(app.registry().set<input_axises>()) {
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
            parse_axismap(jconfig.at("input").at("axismap"), axismap);
        }
    } catch (nlohmann::json::exception& e) {
        spdlog::warn("invalid input config: {}", e.what());
    }

    app.add_system_to_stage<&input_plugin::reset>("pre_event"_hs, *this)
        .add_system_to_stage<&input_plugin::update>("event"_hs, *this);
}

input_plugin::~input_plugin() { dispatcher.disconnect(*this); }

void input_plugin::reset() {
    for (auto& [_, value] : keystates) {
        value.reset();
    }

    actions.update();
}

void input_plugin::update() {
    for (auto&& axis_binding : axismap) {
        float x{}, y{};
        if (auto it = keystates.find(axis_binding.keys[0]);
            it != keystates.cend()) {
            y += it->second.state ? 1.0f : 0.0f;
        }
        if (auto it = keystates.find(axis_binding.keys[1]);
            it != keystates.cend()) {
            x -= it->second.state ? 1.0f : 0.0f;
        }
        if (auto it = keystates.find(axis_binding.keys[2]);
            it != keystates.cend()) {
            y -= it->second.state ? 1.0f : 0.0f;
        }
        if (auto it = keystates.find(axis_binding.keys[3]);
            it != keystates.cend()) {
            x += it->second.state ? 1.0f : 0.0f;
        }
        axises.set_value(axis_binding.action, {x, y});
    }
}

bool input_plugin::handle_ui_keyboard(const event::keyboard_input& e) {
    auto& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        auto key = e.scan_code;
        IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
        io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
        io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
        io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
        io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
        auto not_propagate = true;
        if ((io.KeyCtrl || io.KeyShift || io.KeyAlt || io.KeySuper) &&
            e.scan_code != SDL_SCANCODE_LCTRL &&
            e.scan_code != SDL_SCANCODE_RCTRL &&
            e.scan_code != SDL_SCANCODE_LSHIFT &&
            e.scan_code != SDL_SCANCODE_RSHIFT &&
            e.scan_code != SDL_SCANCODE_LALT &&
            e.scan_code != SDL_SCANCODE_RALT &&
            e.scan_code != SDL_SCANCODE_LGUI &&
            e.scan_code != SDL_SCANCODE_RGUI) {
            const auto is_imgui_shortcut_key =
                io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
            auto name = std::toupper(e.key_code);
            not_propagate = is_imgui_shortcut_key &&
                            (name == 'Z' || name == 'Y' || name == 'C' ||
                             name == 'V' || name == 'X' || name == 'A');
        }
        return not_propagate;
    }
    return false;
}

bool input_plugin::handle_ui_mouse_button(const event::mouse_button_input& e) {
    return ImGui::GetIO().WantCaptureMouse;
}

bool input_plugin::handle_ui_mouse_motion(const event::mouse_motion_input& e) {
    return ImGui::GetIO().WantCaptureMouse;
}

bool input_plugin::handle_ui_mouse_wheel(const event::mouse_wheel_input& e) {
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        if (e.x > 0)
            io.MouseWheelH += 1;
        if (e.x < 0)
            io.MouseWheelH -= 1;
        if (e.y > 0)
            io.MouseWheel += 1;
        if (e.y < 0)
            io.MouseWheel -= 1;
        return true;
    }
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
        if (auto it = keystates.find(e.key_code); it == keystates.cend()) {
            keystates[e.key_code] = {};
        }
        if (e.pressed) {
            keystates[e.key_code].down();
        } else {
            keystates[e.key_code].up();
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

static const char* axismap_text = R"({
    "wasd": { "axis": "move1" },
    "RQPO": { "axis": "look1" }
})";

} // namespace

TEST_CASE("input_plugin: parse keymap") {
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

TEST_CASE("input_plugin: parse axismap") {
    using namespace entt::literals;

    nlohmann::json j = nlohmann::json::parse(axismap_text);

    std::vector<motor::axis_binding> axismap;
    parse_axismap(j, axismap);

    {
        auto it = std::find_if(axismap.cbegin(), axismap.cend(), [](auto&& it) {
            return it.action == "move1"_hs;
        });
        CHECK(it != axismap.cend());
        CHECK(it->keys[0] == 'w');
        CHECK(it->keys[1] == 'a');
        CHECK(it->keys[2] == 's');
        CHECK(it->keys[3] == 'd');
    }

    {
        auto it = std::find_if(axismap.cbegin(), axismap.cend(), [](auto&& it) {
            return it.action == "look1"_hs;
        });
        CHECK(it != axismap.cend());
        CHECK(it->keys[0] == 'R');
        CHECK(it->keys[1] == 'Q');
        CHECK(it->keys[2] == 'P');
        CHECK(it->keys[3] == 'O');
    }
}
