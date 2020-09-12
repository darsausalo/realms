#ifndef MOTOR_MOD_H
#define MOTOR_MOD_H

#include "motor/core/plugin_context.h"
#include "motor/core/system_dispatcher.h"
#include "motor/platform/dynamic_library.h"
#include <filesystem>
#include <string_view>
#include <vector>

namespace motor {

struct mod_manifest {
    std::vector<std::string> dependencies{};

    std::string author{};
    std::string description{};
};

class mod : public plugin_context {
public:
    mod(std::string_view name, const std::filesystem::path& path,
        const mod_manifest& manifest) noexcept
        : name{name}, path{path}, dl{name, std::move(path)},
          manifest{std::move(manifest)} {}

    mod(const mod&) = delete;
    mod(mod&&) = default;

    mod& operator=(mod&) = delete;
    mod& operator=(mod&&) = default;

    ~mod() noexcept;

    const std::string& get_name() const { return name; }
    const std::filesystem::path& get_path() const { return path; }
    const mod_manifest& get_manifest() const { return manifest; }

    bool is_valid() const noexcept;
    bool is_changed() const noexcept;

    bool load_plugin() noexcept;
    void unload_plugin() noexcept;

    void add_systems(system_dispatcher& dispatcher);
    void remove_systems(system_dispatcher& dispatcher);

    std::shared_ptr<spdlog::logger> get_logger() const override;

private:
    dynamic_library dl;

    std::string name{};
    std::filesystem::path path{};
    mod_manifest manifest{};
    std::vector<std::string> dependencies{};
    bool valid{};
};

} // namespace motor

#endif // MOTOR_MOD_H
