#include "motor/resources/image.hpp"
#include "motor/core/filesystem.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace motor {

image::image(std::string_view name)
    : internal_image{} {
    auto path = filesystem::full_path(fmt::format("mods/{}", name));
    path.make_preferred();
    stbi_set_flip_vertically_on_load(true);
    int n;
    auto data = stbi_load(std::data(path.string()), &width, &height, &n, 4);
    if (!data) {
        spdlog::error("failed to load {}", path.string());
    }
    pixels.reset(data);
}

sg_image image::upload() noexcept {
    if (internal_image.id == SG_INVALID_ID) {
        sg_image_desc desc{};
        desc.width = width;
        desc.height = height;
        desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        desc.min_filter = SG_FILTER_NEAREST;
        desc.mag_filter = SG_FILTER_NEAREST;
        desc.content.subimage[0][0] = {pixels.get(), width * height * 4};
        internal_image = sg_make_image(&desc);
    }
    return internal_image;
}


} // namespace motor
