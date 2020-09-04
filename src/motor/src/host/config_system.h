#ifndef CONFIG_SYSTEM_H
#define CONFIG_SYSTEM_H

#include "motor/core/system.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace motor {

namespace event {

struct config_changed {
    std::string key;
    nlohmann::json value;
};

} // namespace event

struct config_data {
    nlohmann::json jconfig;
};

class config_system : public system {
public:
    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    void update(entt::registry& reg) override;

private:
    bool modified{};
    nlohmann::json cli_config{};

    void receive_config_changed(const event::config_changed&);
};

} // namespace motor

#endif // CONFIG_SYSTEM_H
