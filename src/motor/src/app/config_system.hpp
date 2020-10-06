#ifndef CONFIG_SYSTEM_HPP
#define CONFIG_SYSTEM_HPP

#include "core/internal_events.hpp"
#include <entt/entity/fwd.hpp>
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

class config_system {
public:
    config_system(const arg_list& args, entt::registry& registry);
    ~config_system();

private:
    entt::registry& registry;
    nlohmann::json& config;
    bool modified{};

    void receive_config_changed(const event::config_changed&);
};

} // namespace motor

#endif // CONFIG_SYSTEM_HPP
