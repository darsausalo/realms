#ifndef FRONTIER_COMPONENTS_H
#define FRONTIER_COMPONENTS_H

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

} // namespace frontier

#endif // FRONTIER_COMPONENTS_H
