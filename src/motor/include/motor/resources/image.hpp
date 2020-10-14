#ifndef MOTOR_IMAGE_HPP
#define MOTOR_IMAGE_HPP

#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <sokol_gfx.h>
#include <string_view>

namespace motor {

class image {
public:
    explicit image(std::string_view name);

    image(image&&) = default;
    image& operator=(image&&) = default;

    sg_image upload() noexcept;

    glm::vec2 size() const noexcept {
        return {width, height};
    }

private:
    std::int32_t width;
    std::int32_t height;
    std::unique_ptr<std::uint8_t> pixels;
    sg_image internal_image;
};

} // namespace motor

#endif // MOTOR_IMAGE_HPP
