#include "motor/resources/image.hpp"
#include "motor/core/filesystem.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace motor {

image::image(std::string_view name) {
    std::filesystem::path path =
            filesystem::full_path(fmt::format("mods/{}", name));
    path.make_preferred();
    stbi_set_flip_vertically_on_load(true);
    int n;
    auto data = stbi_load(std::data(path.string()), &width, &height, &n, 4);
    if (!data) {
        spdlog::error("failed to load {}", path.string());
    }
    pixels.reset(data);
}

} // namespace motor
