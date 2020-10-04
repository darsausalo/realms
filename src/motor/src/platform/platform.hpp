#ifndef MOTOR_PLATFORM_HPP
#define MOTOR_PLATFORM_HPP

#include <filesystem>
#include <string_view>

namespace motor::platform {

void setup_crash_handling(const std::filesystem::path& base_path);

} // namespace motor::platform

#endif // MOTOR_PLATFORM_HPP
