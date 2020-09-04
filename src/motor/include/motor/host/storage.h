#ifndef MOTOR_STORAGE_H
#define MOTOR_STORAGE_H

#include <filesystem>

namespace motor {

class storage {
public:
    storage(const std::filesystem::path& base_path,
            const std::filesystem::path& data_path,
            const std::filesystem::path& user_path) noexcept;
    storage(storage&) = delete;
    storage(storage&&) = default;

    storage& operator=(storage&) = delete;
    storage& operator=(storage&&) = default;

    ~storage() noexcept = default;

    const std::filesystem::path& get_base_path() const { return base_path; }
    const std::filesystem::path& get_data_path() const { return data_path; }
    const std::filesystem::path& get_user_path() const { return user_path; }

    void set_data_path(const std::filesystem::path& path);
    void set_user_path(const std::filesystem::path& path);

    std::filesystem::path
    get_full_path(const std::filesystem::path& relative_path,
                  bool for_write = false) const;

private:
    std::filesystem::path base_path;
    std::filesystem::path data_path;
    std::filesystem::path user_path;
};

} // namespace motor

#endif // MOTOR_STORAGE_H
