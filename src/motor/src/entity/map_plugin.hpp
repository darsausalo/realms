#ifndef MAP_PLUGIN_HPP
#define MAP_PLUGIN_HPP

#include "motor/app/app_builder.hpp"
#include "motor/entity/map.hpp"

namespace motor {

class map_plugin {
public:
    map_plugin(app_builder& app) { app.define_component<map>(); }
};

} // namespace motor

#endif // MAP_PLUGIN_HPP
