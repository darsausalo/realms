#include "motor/services/components_service.h"
#include "motor/core/prototype_registry.h"

namespace motor {

// TODO: how to properly register engine components?
template<typename Archive>
void serialize(Archive& ar, prototype& value) {
    ar.member(M(value.value));
}

components_service::components_service() noexcept {
    component<prototype>();
}

} // namespace motor
