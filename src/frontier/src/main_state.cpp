#include "main_state.h"

namespace frontier {

void main_state::on_start() {
}

void main_state::on_stop() {
}

motor::transition main_state::update() {
    return motor::transition_none{};
}

} // namespace frontier
