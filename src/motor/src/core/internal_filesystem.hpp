#ifndef MOTOR_INTERNAL_FILESYSTEM_HPP
#define MOTOR_INTERNAL_FILESYSTEM_HPP

#include <filesystem>

namespace motor::filesystem {

void init(const std::filesystem::path& base, const std::filesystem::path& data,
          const std::filesystem::path& user);

} // namespace motor::filesystem

#endif // MOTOR_INTERNAL_FILESYSTEM_HPP
