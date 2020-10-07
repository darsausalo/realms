#ifndef MOTOR_IMAGE_HPP
#define MOTOR_IMAGE_HPP

#include <cstdint>
#include <memory>
#include <string_view>

namespace motor {

class image {
public:
    explicit image(std::string_view name);

    image(image&&) = default;
    image& operator=(image&&) = default;

    std::int32_t get_width() const noexcept { return width; };
    std::int32_t get_height() const noexcept { return height; };

private:
    std::int32_t width;
    std::int32_t height;
    std::unique_ptr<std::uint8_t> pixels;
};

} // namespace motor

#endif // MOTOR_IMAGE_HPP
