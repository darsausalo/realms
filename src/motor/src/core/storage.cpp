#include "motor/core/storage.h"

namespace motor {

storage::storage(const std::filesystem::path& base_path,
                 const std::filesystem::path& data_path,
                 const std::filesystem::path& user_path) noexcept
    : base_path(base_path), data_path(data_path), user_path(user_path) {
}

void storage::set_data_path(const std::filesystem::path& path) {
    data_path = path;
}

void storage::set_user_path(const std::filesystem::path& path) {
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
