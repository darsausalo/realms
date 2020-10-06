#ifndef MOTOR_INPUT_HPP
#define MOTOR_INPUT_HPP

#include <cstdint>
#include <entt/core/hashed_string.hpp>
#include <unordered_map>

namespace motor {

struct cursor_position {
    std::int32_t x;
    std::int32_t y;
};

class input_actions {
public:
    [[nodiscard]] bool is_pressed(entt::id_type name_id) const noexcept {
        if (auto it = pressed.find(name_id); it != pressed.cend()) {
            return it->second;
        }
        return false;
    }

    [[nodiscard]] bool is_just_pressed(entt::id_type name_id) const noexcept {
        if (auto it = just_pressed.find(name_id); it != just_pressed.cend()) {
            return it->second;
        }
        return false;
    }

    [[nodiscard]] bool is_just_released(entt::id_type name_id) const noexcept {
        if (auto it = just_released.find(name_id); it != just_released.cend()) {
            return it->second;
        }
        return false;
    }

    void press(entt::id_type name_id) noexcept {
        if (auto it = pressed.find(name_id);
            it == pressed.cend() || !it->second) {
            just_pressed[name_id] = true;
        }
        pressed[name_id] = true;
    }

    void release(entt::id_type name_id) noexcept {
        pressed[name_id] = false;
        just_released[name_id] = true;
    }

    void update() noexcept {
        just_pressed.clear();
        just_released.clear();
    }

private:
    std::unordered_map<entt::id_type, bool> pressed;
    std::unordered_map<entt::id_type, bool> just_pressed;
    std::unordered_map<entt::id_type, bool> just_released;
};

} // namespace motor

#endif // MOTOR_INPUT_HPP
