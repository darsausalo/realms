#include "transform.hpp"
#include "components/base.hpp"
#include <motor/app/app_builder.hpp>
#include <motor/entity/transform.hpp>

namespace realms {

void emplace_transform(entt::registry& registry, entt::entity e) {
    registry.emplace<motor::transform>(e, glm::mat4{1.0f});
}

void apply_transform(
    entt::view<entt::get_t<const position, motor::transform>> view) {
    for (auto&& e : view) {
        const auto& p = view.get<const position>(e);
        auto& tfm = view.get<motor::transform>(e);
        // TODO: add rotation
        tfm.value[3] = glm::vec4{p.x, p.y, 0.0f, 1.0f};
    }
}

void register_transforms(motor::app_builder& app) {
    app.registry().on_construct<position>().connect<&emplace_transform>();

    app.add_system<&apply_transform>();
}

} // namespace realms
