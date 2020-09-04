#ifndef MOTOR_PLATFORM_H
#define MOTOR_PLATFORM_H

#include <filesystem>
#include <string_view>

namespace motor::platform {

void setup_crash_handling(const std::filesystem::path& base_path);

} // namespace motor::platform

#endif // MOTOR_PLATFORM_H
