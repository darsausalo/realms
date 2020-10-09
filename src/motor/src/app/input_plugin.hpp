#ifndef MOTOR_INPUT_PLUGIN_HPP
#define MOTOR_INPUT_PLUGIN_HPP

#include "motor/core/events.hpp"
#include "motor/core/input.hpp"
#include "motor/core/utility.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>

namespace motor {

struct action_key {
    entt::id_type key;
    std::uint16_t modifiers;

    constexpr bool operator==(const action_key& other) const noexcept {
        return key == other.key && modifiers == other.modifiers;
    }
};

} // namespace motor

namespace std {

template<>
struct hash<motor::action_key> {
    std::size_t operator()(const motor::action_key& value) const noexcept {
        std::size_t h = static_cast<std::size_t>(value.key);
        motor::hash_combine(h, static_cast<std::size_t>(value.modifiers));
        return h;
    }
};

} // namespace std

namespace motor {

class app_builder;

class input_plugin {
public:
    input_plugin(app_builder& app);
    ~input_plugin();

private:
    entt::dispatcher& dispatcher;
    nlohmann::json& jconfig;
    cursor_position& pointer_position;
    input_actions& actions;

    std::unordered_map<action_key, entt::id_type> keymap;

    void update_actions();

    bool handle_ui_keyboard(const event::keyboard_input& e);
    bool handle_ui_mouse_button(const event::mouse_button_input& e);
    bool handle_ui_mouse_motion(const event::mouse_motion_input& e);
    bool handle_ui_mouse_wheel(const event::mouse_wheel_input& e);

    void receive_keyboard_input(const event::keyboard_input& e);
    void receive_mouse_button_input(const event::mouse_button_input& e);
    void receive_mouse_motion_input(const event::mouse_motion_input& e);
    void receive_mouse_wheel_input(const event::mouse_wheel_input& e);
};

} // namespace motor

#endif // MOTOR_INPUT_PLUGIN_HPP
