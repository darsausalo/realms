#ifndef CONFIG_SYSTEM_H
#define CONFIG_SYSTEM_H

#include "motor/core/system.h"
#include "motor/core/type_traits.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace motor {

class arg_list : public std::vector<std::string> {
public:
    arg_list(int argc, const char* argv[]) {
        for (int i = 0; i < argc; i++) {
            if (argv[i]) {
                push_back(argv[i]);
            }
        }
    }
};

using core_config = nlohmann::json;

namespace event {

struct config_changed {
    std::string key;
    nlohmann::json value;
};

} // namespace event

class config_system : public system<system_group::pre_frame> {
public:
    config_system();

    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    void update(entt::registry& reg) override;

private:
    bool modified{};

    void receive_config_changed(const event::config_changed&);
};

} // namespace motor

#endif // CONFIG_SYSTEM_H
