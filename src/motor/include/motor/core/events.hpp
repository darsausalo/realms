#ifndef MOTOR_EVENTS_HPP
#define MOTOR_EVENTS_HPP

#include <cstdint>
#include <string>

namespace motor::event {

struct start {};

struct quit {};

struct respawn {};

struct keyboard_input {
    std::uint16_t scan_code;
    std::uint16_t key_code;
    bool pressed;
    bool released;
    bool repeat;
};

struct text_input {
    std::string text;
};

struct mouse_button_input {
    std::uint8_t key; // 0 - left, 1 - right
    std::uint8_t clicks;
    bool pressed;
};

struct mouse_motion_input {
    std::int32_t x;
    std::int32_t y;
    std::int32_t dx;
    std::int32_t dy;
};

struct mouse_wheel_input {
    std::int32_t x;
    std::int32_t y;
};

} // namespace motor::event

#endif // MOTOR_EVENTS_HPP
