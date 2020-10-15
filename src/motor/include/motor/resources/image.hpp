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

    [[no_discard]] sg_image upload() noexcept;

    [[no_discard]] glm::vec2 size() const noexcept {
        return {width, height};
    }

    [[no_discard]] glm::vec2 atlas_origin() const noexcept {
        return glm::vec2{0.0f, 0.0f};
    }

    [[no_discard]] glm::vec2 atlas_size() const noexcept {
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
