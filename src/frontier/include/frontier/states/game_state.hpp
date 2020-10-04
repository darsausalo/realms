#ifndef FRONTIER_MAIN_STATE_H
#define FRONTIER_MAIN_STATE_H

#include <motor/host/state.h>

namespace frontier {

class main_state : public motor::state {
public:
    main_state(entt::registry& reg);
    ~main_state();

    motor::transition update() override;
};

} // namespace frontier


#endif // FRONTIER_MAIN_STATE_H
