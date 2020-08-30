#ifndef FRONTIER_MAIN_STATE_H
#define FRONTIER_MAIN_STATE_H

#include <motor/core/game_state.h>

namespace frontier {

class main_state : public motor::game_state {
public:
    main_state() noexcept = default;
    ~main_state() noexcept = default;

    void on_start() override;
    void on_stop() override;
    motor::transition update() override;
};

} // namespace frontier


#endif // FRONTIER_MAIN_STATE_H
