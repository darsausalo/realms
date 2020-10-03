#ifndef FRONTIER_COMPONENTS_H
#define FRONTIER_COMPONENTS_H

#include <entt/core/type_info.hpp>
#include <string>

namespace frontier {

struct position {
    float x;
    float y;
};

struct velocity {
    float dx;
    float dy;
};

struct health {
    int max;
    int value;
};

struct sprite {
    std::string resource;
};

} // namespace frontier

#endif // FRONTIER_COMPONENTS_H
