#ifndef MOTOR_INPUT_HPP
#define MOTOR_INPUT_HPP

#include <cstdint>
#include <entt/core/fwd.hpp>
#include <glm/common.hpp>
#include <glm/vec2.hpp>
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

class input_axises {
public:
    [[nodiscard]] glm::vec2 get_value(entt::id_type name_id) const {
        if (auto it = values.find(name_id); it != values.cend()) {
            return it->second;
        }
        return {};
    }

    void add_value(entt::id_type name_id, const glm::vec2& value) {
        if (auto it = values.find(name_id); it == values.cend()) {
            values[name_id] = {};
        }
        values[name_id] =
            glm::clamp(values[name_id] + value, glm::vec2{-1.0f, -1.0f},
                       glm::vec2{1.0f, 1.0f});
    }

    void reset() {
        for (auto&& [_, value] : values) {
            value = {};
        }
    }

private:
    std::unordered_map<entt::id_type, glm::vec2> values;
};

} // namespace motor

#endif // MOTOR_INPUT_HPP
