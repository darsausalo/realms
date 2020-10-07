#include "motor/core/filesystem.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace motor::filesystem {

static std::filesystem::path base_path;
static std::filesystem::path data_path;
static std::filesystem::path user_path;

void init(const std::filesystem::path& base, const std::filesystem::path& data,
          const std::filesystem::path& user) {
    base_path = base;
    data_path = data;
    user_path = user;

    base_path.make_preferred();
    data_path.make_preferred();
    user_path.make_preferred();

    if (!std::filesystem::exists(data_path)) {
        throw std::runtime_error(fmt::format(
                "data directory '{}' was not found", data_path.string()));
    }
    if (!std::filesystem::exists(user_path)) {
        std::error_code ec;
        if (!std::filesystem::create_directories(user_path)) {
            user_path = data_path;
            spdlog::error("failed to create user directory '{}': {}",
                          user_path.string(), ec.message());
        }
    }
}

std::filesystem::path full_path(const std::filesystem::path& relative_path,
                                bool for_write) {
    if (for_write) {
        if (!std::filesystem::exists(user_path)) {
            return data_path / relative_path;
        }
        return user_path / relative_path;
    }
    auto path = user_path / relative_path;
    if (std::filesystem::exists(path)) {
        return path;
    }
    return data_path / relative_path;
}

const std::filesystem::path& base() {
    return base_path;
}
const std::filesystem::path& data() {
    return data_path;
}
const std::filesystem::path& user() {
    return user_path;
}

} // namespace motor::filesystem
