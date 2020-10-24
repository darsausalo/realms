#ifndef MOTOR_IMAGE_ATLAS_HPP
#define MOTOR_IMAGE_ATLAS_HPP

#include "motor/core/rect.hpp"
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <sokol_gfx.h>

namespace motor {

class image_atlas {
    friend class image;

public:
    image_atlas(std::size_t width, std::size_t height) noexcept;

    [[no_discard]] sg_image resource() const noexcept { return res; }
    [[no_discard]] glm::vec2 size() const noexcept { return {width, height}; }

    void upload() noexcept;

    void save() noexcept;

private:
    std::size_t id;
    std::size_t width;
    std::size_t height;
    std::unique_ptr<std::uint8_t[]> pixels{};

    sg_image res{};
};

} // namespace motor

#endif // MOTOR_IMAGE_ATLAS_HPP
