#ifndef FRONTIER_COMPONENTS_H
#define FRONTIER_COMPONENTS_H

#include <entt/resource/handle.hpp>
#include <motor/resources/image.hpp>

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
    entt::resource_handle<motor::image> image;
};

} // namespace frontier

#endif // FRONTIER_COMPONENTS_H
