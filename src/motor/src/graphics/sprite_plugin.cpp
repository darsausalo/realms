#include "sprite_plugin.hpp"
#include "graphics/pipeline.hpp"
#include "motor/app/app_builder.hpp"
#include <algorithm>
#include <array>
#include <entt/entity/registry.hpp>

namespace motor {

sprite_plugin::sprite_plugin(app_builder& app) {
    init_pipeline(pipeline, bindings);

    app.define_component<sprite>()
        .define_component<sprite_sheet>()
        .add_system_to_stage<&sprite_plugin::prepare_sprites>(
            "pre_render"_hs, *this)
        .add_system_to_stage<&sprite_plugin::prepare_sprite_sheets>(
            "pre_render"_hs, *this)
        .add_system_to_stage<&sprite_plugin::update_sprite_sheets>(
            "pre_render"_hs, *this)
        .add_system_to_stage<&sprite_plugin::emplace_sprites>(
            "pre_render"_hs, *this)
        .add_system_to_stage<&sprite_plugin::render_sprites>(
            "render"_hs, *this);

    app.registry().set<camera2d>(glm::vec2{0.0f, 0.0f}, 1.0f);
}

void sprite_plugin::prepare_sprites(
    entt::view<entt::get_t<sprite>, entt::exclude_t<sg_image>> view,
    entt::registry& registry) {
    view.each([&registry](auto e, auto& s) {
        if (s.image->valid()) {
            registry.emplace<sg_image>(e, s.image->resource());
            registry.emplace<image_region>(e, s.image->origin(),
                                           s.image->size(), s.image->origin(),
                                           s.image->atlas_size());
        }
    });
}

void sprite_plugin::prepare_sprite_sheets(
    entt::view<entt::get_t<sprite_sheet>, entt::exclude_t<sg_image>> view,
    entt::registry& registry) {
    view.each([&registry](auto e, auto& s) {
        if (s.image->valid()) {
            registry.emplace<sg_image>(e, s.image->resource());
            registry.emplace<image_region>(
                e, s.image->origin(),
                s.image->size() / glm::vec2{s.columns, s.rows},
                s.image->origin(), s.image->atlas_size());
        }
    });
}

void sprite_plugin::update_sprite_sheets(
    entt::view<entt::get_t<const sprite_sheet, image_region>> view) {
    view.each([](auto e, auto& s, auto& r) {
        auto column = s.index % s.columns;
        auto row = (s.index / s.columns) % s.rows;
        r.origin = r.atlas_origin + glm::vec2{column, row} * r.size;
    });
}

void sprite_plugin::emplace_sprites(
    entt::view<entt::get_t<const sg_image, const transform, const image_region>,
               entt::exclude_t<entt::tag<"hidden"_hs>>> view) {
    sprites.clear();
    view.each([this](const auto& image, const auto& tfm, const auto& r) {
        sprites.push_back({
            image,
            tfm.value,
            {
                glm::vec2{r.origin.x, r.origin.y + r.size.y},
                glm::vec2{r.origin.x + r.size.x, r.origin.y + r.size.y},
                glm::vec2{r.origin.x + r.size.x, r.origin.y},
                glm::vec2{r.origin.x, r.origin.y},
            },
            r.atlas_size,
        });
    });

    std::sort(
        sprites.begin(), sprites.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.tfm[3].z < lhs.tfm[3].z;
        });
}

void sprite_plugin::render_sprites(const screen& screen,
                                   const camera2d& camera) {
    if (sprites.empty()) {
        return;
    }

    vs_params_t vs_params{glm::vec2{screen.width, screen.height},
                          camera.position, std::max(camera.zoom, 1.0f)};

    sg_apply_pipeline(pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &vs_params, sizeof(vs_params_t));

    sg_image batch_image{};
    std::size_t batch_start{}, batch_element{};
    for (std::size_t i{}; i < sprites.size(); ++i) {
        sg_image image = sprites[i].image;
        if (image.id != batch_image.id) {
            if (i > batch_start) {
                render_batch(batch_image, &sprites[batch_start],
                             i - batch_start, batch_element);
            }

            batch_image = image;
            batch_start = i;
        }
    }

    render_batch(batch_image, &sprites[batch_start],
                 sprites.size() - batch_start, batch_element);
}

void sprite_plugin::render_batch(sg_image image,
                                 const sprite_data* sprites,
                                 std::size_t count,
                                 std::size_t& base_element) {
    static std::array<glm::vec2, 4> corners = {
        glm::vec2{-0.5f, -0.5f},
        glm::vec2{0.5f, -0.5f},
        glm::vec2{0.5f, 0.5f},
        glm::vec2{-0.5f, 0.5f},
    };
    std::array<vertex, 4> vertices;
    auto vbuf_info = sg_query_buffer_info(bindings.vertex_buffers[0]);
    while (count > 0) {
        auto batch_size = std::min(count, max_batch_size);
        for (std::size_t i{}; i < batch_size; ++i) {
            auto& sprite = sprites[i];
            auto size = (sprite.rect[1] - sprite.rect[3]);
            for (std::size_t j{}; j < sprite.rect.size(); ++j) {
                vertices[j].position =
                    sprite.tfm * glm::vec4(corners[j] * size, 0.0f, 1.0f);
                vertices[j].uv = sprite.rect[j] / sprite.atlas_size;
                vertices[j].color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
            }
            sg_append_buffer(bindings.vertex_buffers[0], &vertices[0],
                             vertices.size() * sizeof(vertex));
        }
        bindings.fs_images[0] = image;
        sg_apply_bindings(&bindings);
        sg_draw(base_element, batch_size * 6, 1);
        base_element += batch_size * 6;

        sprites += batch_size;
        count -= batch_size;
    }
}

} // namespace motor
