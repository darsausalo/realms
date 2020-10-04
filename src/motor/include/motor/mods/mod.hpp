#ifndef MOTOR_MOD_HPP
#define MOTOR_MOD_HPP

#include <filesystem>
#include <string_view>
#include <vector>

namespace motor {

struct mod_manifest {
    std::vector<std::string> dependencies{};

    std::string author{};
    std::string description{};
};

class mod {
public:
    mod(std::string_view name, const std::filesystem::path& path,
        const mod_manifest& manifest) noexcept
        : name{name}, path{path}, manifest{std::move(manifest)} {}

    const std::string& get_name() const { return name; }
    const std::filesystem::path& get_path() const { return path; }
    const mod_manifest& get_manifest() const { return manifest; }

private:
    std::string name{};
    std::filesystem::path path{};
    mod_manifest manifest{};
};

} // namespace motor

#endif // MOTOR_MOD_HPP
