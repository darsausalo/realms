#ifndef MOTOR_RESOURCES_HPP
#define MOTOR_RESOURCES_HPP

#include <entt/resource/cache.hpp>

namespace motor {

class image;

struct resources {
    resources() = delete;
    ~resources() = delete;

    inline static entt::resource_cache<image> image;
};

} // namespace motor

#endif // MOTOR_RESOURCES_HPP
