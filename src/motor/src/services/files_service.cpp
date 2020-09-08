#include "motor/services/files_service.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace motor {

files_service::files_service(const std::filesystem::path& base_path,
                             const std::filesystem::path& data_path,
                             const std::filesystem::path& user_path)
    : base_path{base_path}, data_path{data_path}, user_path{user_path} {
    this->base_path.make_preferred();
    this->data_path.make_preferred();
    this->user_path.make_preferred();

    if (!std::filesystem::exists(this->data_path)) {
        throw std::runtime_error(fmt::format(
                "data directory '{}' was not found", this->data_path.string()));
    }
    if (!std::filesystem::exists(this->user_path)) {
        std::error_code ec;
        if (!std::filesystem::create_directories(this->user_path)) {
            this->user_path = this->data_path;
            spdlog::error("failed to create user directory '{}': {}",
                          this->user_path.string(), ec.message());
        }
    }

    spdlog::info("base path: {}", this->base_path.string());
    spdlog::info("data path: {}", this->data_path.string());
    spdlog::info("user path: {}", this->user_path.string());
}

std::filesystem::path
files_service::get_full_path(const std::filesystem::path& relative_path,
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
