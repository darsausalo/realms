#ifndef MOTOR_RECT_HPP
#define MOTOR_RECT_HPP

#include <cstddef>

namespace motor {

struct rect {
    std::size_t x;
    std::size_t y;
    std::size_t w;
    std::size_t h;
};

} // namespace motor

#endif // MOTOR_RECT_HPP
