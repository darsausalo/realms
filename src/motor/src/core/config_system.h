#ifndef CONFIG_SYSTEM_H
#define CONFIG_SYSTEM_H

#include "motor/systems/system.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace motor {

class platform;

struct config_changed {
    std::string key;
    nlohmann::json value;
};

class config_system : public system {
public:
    config_system(const std::vector<std::string>& args, platform& platform);

    void on_start(game_data& data) override;
    void on_stop(game_data& data) override;
    void update(game_data& data) override;

    const nlohmann::json& get_config() const { return config; }

private:
    bool modified{};
    nlohmann::json config{};
    nlohmann::json cli_config{};

    const platform& platform;

    void receive_config_changed(const config_changed& event);
};

} // namespace motor

#endif // CONFIG_SYSTEM_H
