#include "motor/resources/image.hpp"
#include "motor/core/filesystem.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace motor {

image::image(std::string_view name)
    : pixels{nullptr, nullptr} {
    auto path = filesystem::full_path(fmt::format("mods/{}", name));
    path.make_preferred();
    stbi_set_flip_vertically_on_load(true);
    int w, h, n;
    auto data = stbi_load(std::data(path.string()), &w, &h, &n, 4);
    if (data) {
        pixels = {data, [](void* instance) {
                      stbi_image_free(static_cast<stbi_uc*>(instance));
                  }};
    } else {
        w = 2;
        h = 2;
        data = new uint8_t[w * h * 4];
        std::memset(data, 0u, w * h * 4 * sizeof(uint8_t));
        pixels = {data, [](void* instance) {
                      delete[] reinterpret_cast<uint8_t*>(instance);
                  }};
        spdlog::error("failed to load {}", path.string());
    }

    region.w = static_cast<std::size_t>(w);
    region.h = static_cast<std::size_t>(h);
}

void image::stamp_into(std::shared_ptr<image_atlas> atlas,
                       const rect& rect) noexcept {
    const auto pixel_stride = 4u * sizeof(uint8_t);
    const auto image_stride = rect.w * pixel_stride;
    const auto atlas_stride = atlas->width * pixel_stride;
    const auto altas_offset = rect.x * pixel_stride;

    for (std::size_t row = rect.y, y = 0u; y < rect.h; ++row, ++y) {
        void* row_ptr =
            atlas->pixels.get() + (row * atlas_stride + altas_offset);
        std::memcpy(row_ptr,
                    static_cast<uint8_t*>(pixels.get()) + y * image_stride,
                    image_stride);
    }

    region.x = rect.x;
    region.y = rect.y;
    atl = atlas;

    pixels.release();
}

} // namespace motor
