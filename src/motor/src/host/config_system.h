#ifndef CONFIG_SYSTEM_H
#define CONFIG_SYSTEM_H

#include "motor/core/system.h"
#include "motor/core/type_traits.h"
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

class config_system : public init_system {
public:
    config_system();

    void on_start(entt::registry& reg) override;
    void on_stop(entt::registry& reg) override;
    void update(entt::registry& reg) override;

private:
    bool modified{};

    void receive_config_changed(const event::config_changed&);
};

template<>
struct is_host_type<config_system> : std::true_type {};

} // namespace motor

#endif // CONFIG_SYSTEM_H
