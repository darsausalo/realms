#ifndef CONFIG_SYSTEM_H
#define CONFIG_SYSTEM_H

#include "motor/core/system.h"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace motor {

using arg_list = std::vector<std::string>;
using core_config = nlohmann::json;

namespace event {

struct config_changed {
    std::string key;
    nlohmann::json value;
};

} // namespace event

class config_system : public system {
public:
    config_system();
    ~config_system();

    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    void update(entt::registry& reg) override;

private:
    std::filesystem::path base_path;
    std::filesystem::path data_path;
    std::filesystem::path user_path;

    bool modified{};

    void receive_config_changed(const event::config_changed&);
};

} // namespace motor

#endif // CONFIG_SYSTEM_H