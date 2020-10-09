#ifndef CONFIG_PLUGIN_HPP
#define CONFIG_PLUGIN_HPP

#include "core/internal_events.hpp"
#include <entt/entity/fwd.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace motor {

class app_builder;

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

class config_plugin {
public:
    config_plugin(const arg_list& args, app_builder& app);
    ~config_plugin();

private:
    nlohmann::json& config;

    void receive(const event::config_changed&);
};

} // namespace motor

#endif // CONFIG_PLUGIN_HPP
