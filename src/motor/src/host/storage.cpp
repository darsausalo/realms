#include "motor/host/storage.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace motor {

storage::storage(const std::filesystem::path& base_path,
                 const std::filesystem::path& data_path,
                 const std::filesystem::path& user_path) noexcept
    : base_path{base_path}, data_path{data_path}, user_path{user_path} {
}

void storage::set_data_path(const std::filesystem::path& path) {
    data_path = path;
    if (!std::filesystem::exists(data_path)) {
        throw std::runtime_error(fmt::format(
                "data directory '{}' was not found", data_path.string()));
    }
}

void storage::set_user_path(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        try {
            std::filesystem::create_directories(path);
        } catch (std::filesystem::filesystem_error& e) {
            user_path = data_path;
            spdlog::error("failed to create directory '{}': {}",
                          user_path.string(), e.what());
            return;
        }
    }
    user_path = path;
}

std::filesystem::path
storage::get_full_path(const std::filesystem::path& relative_path,
                       bool for_write) const {
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

} // namespace motor
