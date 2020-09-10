#ifndef MOTOR_MOD_H
#define MOTOR_MOD_H

#include "motor/core/binary_archive.h"
#include "motor/core/plugin_context.h"
#include "motor/core/prefab_loader.h"
#include "motor/core/system_dispatcher.h"
#include "motor/platform/dynamic_library.h"
#include <entt/entity/fwd.hpp>
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
    mod(std::string_view name, const std::filesystem::path& dir,
        const mod_manifest& manifest) noexcept
        : name{name}, dl{name, std::move(dir)}, manifest{std::move(manifest)} {}

    mod(const mod&) = delete;
    mod(mod&&) = default;

    mod& operator=(mod&) = delete;
    mod& operator=(mod&&) = default;

    ~mod() noexcept;

    const std::string& get_name() const { return name; }
    const mod_manifest& get_manifest() const { return manifest; }
    const std::vector<std::string>& get_dependencies() const {
        return manifest.dependencies;
    }

    bool is_valid() const noexcept;
    bool is_changed() const noexcept;

    bool load_plugin() noexcept;
    void unload_plugin() noexcept;

    void add_systems(system_dispatcher& dispatcher);
    void remove_systems(system_dispatcher& dispatcher);

    void load_prefabs(prefab_loader& loader);
    void load_snapshot(const entt::snapshot_loader& ss_loadr,
                       binary_input_archive& ar);
    void save_snapshot(const entt::snapshot& ss, binary_output_archive& ar);

    std::shared_ptr<spdlog::logger> get_logger() const override;

protected:
    void add_module(std::string_view module_name,
                    std::unique_ptr<system_module>&& module_instance) override;

private:
    struct module_desc {
        std::string name{};
        std::unique_ptr<system_module> instance{};

        module_desc() = default;
        module_desc(std::string_view name,
                    std::unique_ptr<system_module>&& instance)
            : name{name}, instance{std::move(instance)} {}
    };

    dynamic_library dl;

    std::string name{};
    mod_manifest manifest{};
    std::vector<std::string> dependencies{};
    std::vector<module_desc> modules{};
    bool valid{};
};

} // namespace motor

#endif // MOTOR_MOD_H