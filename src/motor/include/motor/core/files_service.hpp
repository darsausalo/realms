#ifndef MOTOR_FILES_SERVICE_H
#define MOTOR_FILES_SERVICE_H

#include <filesystem>

namespace motor {

class files_service {
public:
    files_service(const std::filesystem::path& base_path,
                  const std::filesystem::path& data_path,
                  const std::filesystem::path& user_path);
    files_service(files_service&) = delete;
    files_service(files_service&&) = delete;

    files_service& operator=(files_service&) = delete;
    files_service& operator=(files_service&&) = delete;

    ~files_service() noexcept = default;

    const std::filesystem::path& get_base_path() const { return base_path; }
    const std::filesystem::path& get_data_path() const { return data_path; }
    const std::filesystem::path& get_user_path() const { return user_path; }

    std::filesystem::path
    get_full_path(const std::filesystem::path& relative_path,
                  bool for_write = false) const;

private:
    std::filesystem::path base_path;
    std::filesystem::path data_path;
    std::filesystem::path user_path;
};

} // namespace motor

#endif // MOTOR_FILES_SERVICE_H
