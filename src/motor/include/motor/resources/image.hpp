#ifndef MOTOR_IMAGE_HPP
#define MOTOR_IMAGE_HPP

#include "motor/core/rect.hpp"
#include "motor/resources/image_atlas.hpp"
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <sokol_gfx.h>
#include <string_view>

namespace motor {

class image {
    friend class image_atlas;

public:
    explicit image(std::string_view name);

    image(image&&) = default;
    image& operator=(image&&) = default;

    [[no_discard]] std::int32_t width() const noexcept { return region.w; }
    [[no_discard]] std::int32_t height() const noexcept { return region.h; }

    [[no_discard]] sg_image resource() const noexcept {
        return atl ? atl->resource() : sg_image{};
    }
    [[no_discard]] glm::vec2 atlas_size() const noexcept {
        return atl ? atl->size() : size();
    }

    [[no_discard]] glm::vec2 origin() const noexcept {
        return {region.x, region.y};
    }
    [[no_discard]] glm::vec2 size() const noexcept {
        return {region.w, region.h};
    }

    [[no_discard]] bool valid() const noexcept { return atl != nullptr; }

    void stamp_into(std::shared_ptr<image_atlas> atlas,
                    const rect& rect) noexcept;

private:
    std::unique_ptr<void, void (*)(void*)> pixels;

    std::shared_ptr<image_atlas> atl{};
    rect region{};
};

} // namespace motor

#endif // MOTOR_IMAGE_HPP
