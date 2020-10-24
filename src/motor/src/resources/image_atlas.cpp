#include "motor/resources/image_atlas.hpp"
#include "motor/core/filesystem.hpp"
#include "motor/resources/image.hpp"
#include <fmt/format.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

static std::size_t last_id = 1u;

namespace motor {

image_atlas::image_atlas(std::size_t width, std::size_t height) noexcept
    : id{last_id++}
    , width(width)
    , height(height) {
    auto size = width * height * 4u;
    pixels = std::make_unique<uint8_t[]>(size);
    std::memset(pixels.get(), 0u, size);
}

void image_atlas::upload() noexcept {
    assert(res.id == SG_INVALID_ID);
    sg_image_desc desc{};
    desc.width = width;
    desc.height = height;
    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.min_filter = SG_FILTER_NEAREST;
    desc.mag_filter = SG_FILTER_NEAREST;
    desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
    desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    desc.content.subimage[0][0] = {
        pixels.get(), static_cast<int>(width * height * 4u)};
    res = sg_make_image(&desc);
}

void image_atlas::save() noexcept {
    static constexpr const auto pixel_stride = 4u * sizeof(uint8_t);

    const auto path =
        filesystem::full_path(fmt::format("test_atlas-{}.tga", id), true);
    stbi_write_tga(std::data(path.string()), width, height, 4, pixels.get());
}

} // namespace motor
