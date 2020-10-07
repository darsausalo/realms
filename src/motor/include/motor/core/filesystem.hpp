#ifndef MOTOR_FILESYSTEM_HPP
#define MOTOR_FILESYSTEM_HPP

#include <filesystem>

namespace motor::filesystem {

const std::filesystem::path& base();
const std::filesystem::path& data();
const std::filesystem::path& user();

std::filesystem::path full_path(const std::filesystem::path& relative_path,
                                bool for_write = false);

} // namespace motor::filesystem

#endif // MOTOR_FILESYSTEM_HPP
