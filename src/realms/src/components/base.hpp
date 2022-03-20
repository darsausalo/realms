#ifndef REALMS_COMPONENTS_H
#define REALMS_COMPONENTS_H

namespace realms {

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

} // namespace realms

#endif // REALMS_COMPONENTS_H
