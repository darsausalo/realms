#include "tile_plugin.hpp"
#include "graphics/pipeline.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/entity/map_region.hpp"
#include "motor/resources/image_loader.hpp"
#include <glm/common.hpp>

namespace motor {

tile_plugin::tile_plugin(app_builder& app) {
    init_pipeline(pipeline, bindings);

    app.define_component<tile_set>()
        .define_component<tile_chunk>()
        .add_system_to_stage<&tile_plugin::update_tilesets>(
            "pre_render"_hs, *this)
        .add_system_to_stage<&tile_plugin::update_tiles>("pre_render"_hs, *this)
        .add_system_to_stage<&tile_plugin::render_tiles>("render"_hs, *this);
}

void tile_plugin::update_tilesets(
    entt::view<entt::get_t<const map, tile_set>> view) {
    for (auto&& [_, m, ts] : view.each()) {
        if (ts.tiles.empty()) {
            continue;
        }

        // prepare internal structure
        if (ts.images.empty()) {
            ts.images.resize(ts.tiles.size());
            ts.dims.resize(ts.tiles.size());
            ts.regions.resize(ts.tiles.size());
            ts.indices.resize(ts.tiles.size());
            ts.rects.resize(ts.tiles.size());

            for (std::size_t i{}; i < ts.tiles.size(); ++i) {
                ts.images[i] = ts.tiles[i]->resource();
            }
            for (std::size_t i{}; i < ts.tiles.size(); ++i) {
                ts.dims[i] = glm::uvec2{
                    std::max(1.0f, ts.tiles[i]->size().x / m.tile_size.x),
                    std::max(1.0f, ts.tiles[i]->size().y / m.tile_size.y),
                };
            }
            for (std::size_t i{}; i < ts.tiles.size(); ++i) {
                ts.regions[i] = image_region{
                    ts.tiles[i]->origin(),
                    ts.tiles[i]->size() / glm::vec2{ts.dims[i].x, ts.dims[i].y},
                    ts.tiles[i]->origin(),
                    ts.tiles[i]->atlas_size(),
                };
            }
        }

        // update uv's for tiles (rects)
        for (std::size_t i{}; i < ts.tiles.size(); ++i) {
            auto column = ts.indices[i] % ts.dims[i].x;
            auto row = (ts.indices[i] / ts.dims[i].x) % ts.dims[i].y;
            const auto& r = ts.regions[i];
            auto origin = r.atlas_origin + glm::vec2{column, row} * r.size;
            ts.rects[i] = {
                glm::vec2{origin.x, origin.y + r.size.y} / r.atlas_size,
                glm::vec2{origin.x + r.size.x, origin.y + r.size.y} /
                    r.atlas_size,
                glm::vec2{origin.x + r.size.x, origin.y} / r.atlas_size,
                glm::vec2{origin.x, origin.y} / r.atlas_size,
            };
        }
    }
}

void tile_plugin::update_tiles(entt::view<entt::get_t<
                                          const parent,
                                          const tile_chunk,
                                          const transform>> view,
                               const entt::registry& registry,
                               const screen& screen,
                               const camera2d& camera) {
    static std::array<glm::vec2, 4> corners = {
        glm::vec2{-0.5f, -0.5f},
        glm::vec2{0.5f, -0.5f},
        glm::vec2{0.5f, 0.5f},
        glm::vec2{-0.5f, 0.5f},
    };

    std::array<vertex, 4> vertices;
    std::array<glm::vec2, 4> rect;
    std::size_t current_element{};
    sg_image chunk_image{};

    chunks.clear();
    for (auto&& [_, p, tc, tfm] : view.each()) {
        const auto& m = registry.get<map>(p.value);
        const auto& ts = registry.get<tile_set>(p.value);

        glm::vec2 hs{
            (screen.width + m.tile_size.x * 2) / 2.0f / camera.zoom,
            (screen.height + m.tile_size.y * 2) / 2.0f / camera.zoom,
        };
        glm::vec2 org{camera.position - glm::vec2{tfm.value[3]}};

        glm::uvec2 min{
            glm::clamp(glm::ivec2{(org - hs) / m.tile_size} - glm::ivec2{1},
                       glm::ivec2{0}, glm::ivec2{m.chunk_size})};
        glm::uvec2 max{
            glm::clamp(glm::ivec2{(org + hs) / m.tile_size} + glm::ivec2{1},
                       glm::ivec2{0}, glm::ivec2{m.chunk_size})};

        std::size_t chunk_start{}, chunk_size{};
        for (auto&& coord : map_region{min, max}) {
            auto i = coord.y * m.chunk_size.x + coord.x;
            auto t = tc.tiles[i];
            assert(t >= 0u && t < ts.tiles.size());
            if (t == 0u || ts.images[t].id == SG_INVALID_ID) {
                continue;
            }
            auto image = ts.images[t];
            if (image.id != chunk_image.id) {
                if (current_element > chunk_start) {
                    chunks.push_back({
                        tc.layer,
                        chunk_image,
                        chunk_start * 6,
                        (current_element - chunk_start) * 6,
                    });
                }

                chunk_image = image;
                chunk_start = current_element;
            }

            for (std::size_t j{}; j < 4; ++j) {
                vertices[j].position =
                    tfm.value * glm::vec4(corners[j] * m.tile_size +
                                              glm::vec2{coord} * m.tile_size,
                                          0.0f, 1.0f);
                vertices[j].uv = ts.rects[t][j];
                vertices[j].color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
            }
            sg_append_buffer(
                bindings.vertex_buffers[0], &vertices[0], 4 * sizeof(vertex));

            current_element++;
        }

        chunks.push_back({
            tc.layer,
            chunk_image,
            chunk_start * 6,
            (current_element - chunk_start) * 6,
        });
    };

    std::sort(
        chunks.begin(), chunks.end(), [](const auto& lhs, const auto& rhs) {
            if (lhs.layer == rhs.layer) {
                return lhs.image.id < rhs.image.id;
            }
            return lhs.layer < rhs.layer;
        });
}

void tile_plugin::render_tiles(const screen& screen, const camera2d& camera) {
    if (chunks.empty()) {
        return;
    }

    vs_params_t vs_params{glm::vec2{screen.width, screen.height},
                          camera.position, std::max(camera.zoom, 1.0f)};

    sg_apply_pipeline(pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &vs_params, sizeof(vs_params_t));

    for (auto&& chunk : chunks) {
        bindings.fs_images[0] = chunk.image;
        sg_apply_bindings(&bindings);
        sg_draw(chunk.start_element, chunk.element_count, 1);
    }
}

} // namespace motor
