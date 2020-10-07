#ifndef MOTOR_SCRIPTS_HPP
#define MOTOR_SCRIPTS_HPP

#include <filesystem>
#include <sol/sol.hpp>
#include <vector>

namespace motor::scripts {

class error : public std::runtime_error {
public:
    explicit error(std::string_view message)
        : std::runtime_error{std::data(message)} {}
};

void run(sol::state& lua,
         const std::vector<std::filesystem::path>& script_paths,
         const std::vector<std::string_view>& libs,
         const std::vector<std::string_view>& preloads);

} // namespace motor::scripts

#endif // MOTOR_SCRIPTS_HPP
