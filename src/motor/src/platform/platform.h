#ifndef MOTOR_PLATFORM_H
#define MOTOR_PLATFORM_H

#include <filesystem>
#include <string_view>

namespace motor {

class platform {
public:
    platform();
    ~platform();

    const std::filesystem::path& get_base_path() const { return base_path; }
    const std::filesystem::path& get_data_path() const { return data_path; }
    const std::filesystem::path& get_user_path() const { return user_path; }

private:
    std::filesystem::path base_path;
    std::filesystem::path data_path;
    std::filesystem::path user_path;

    void setup_crash_handling(std::string_view base_path);
};


} // namespace motor

#endif // MOTOR_PLATFORM_H
