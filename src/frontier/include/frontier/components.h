#ifndef FRONTIER_COMPONENTS_H
#define FRONTIER_COMPONENTS_H

#include <entt/core/type_info.hpp>

namespace frontier {

struct position {
    float x;
    float y;
};

struct velocity {
    float dx;
    float dy;
};

} // namespace frontier

template<typename Type>
struct entt::type_index<Type> {};

#endif // FRONTIER_COMPONENTS_H
