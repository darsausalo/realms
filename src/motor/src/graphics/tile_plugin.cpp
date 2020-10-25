#include "tile_plugin.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/entity/map_region.hpp"
#include "motor/resources/image_loader.hpp"
#include <glm/common.hpp>

namespace motor {

namespace {

static constexpr const char* vs_source = R"(
#version 330

uniform vec2 screen_size;
uniform vec2 camera_position;
uniform float camera_zoom;

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord0;
layout(location=2) in vec4 color0;

out vec4 color;
out vec2 uv;

void main() {
    vec2 result_position = (((position + 0.5) - camera_position) * camera_zoom);
    gl_Position =
        vec4(((result_position / screen_size)) * vec2(2.0, 2.0), 0.0, 1.0);

    color = color0;
    uv = texcoord0;
}
)";

static constexpr const char* fs_source = R"(
#version 330

uniform sampler2D tex;

in vec4 color;
in vec2 uv;
out vec4 frag_color;

void main() {
    frag_color = texture(tex, uv) * color;
}
)";

static constexpr const std::size_t max_batch_size = 1024 * 16;
static constexpr const std::size_t max_vertices = max_batch_size * 4;
static constexpr const std::size_t max_indices = max_batch_size * 6;

struct vertex {
    glm::vec2 position;
    glm::vec2 uv;
    glm::vec4 color;
};

struct vs_params_t {
    glm::vec2 screen_size;
    glm::vec2 camera_position;
    float camera_zoom;
};

} // namespace

static void init_pipeline(sg_pipeline& pipeline, sg_bindings& bindings) {
    sg_buffer_desc vbuf_desc{};
    vbuf_desc.size = max_vertices * sizeof(vertex);
    vbuf_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
    vbuf_desc.usage = SG_USAGE_STREAM;
    vbuf_desc.size = max_vertices * sizeof(vertex);
    bindings.vertex_buffers[0] = sg_make_buffer(vbuf_desc);

    std::vector<std::uint16_t> indices;
    indices.resize(max_indices);
    for (std::size_t i = 0u; i < max_batch_size; ++i) {
        indices[(i * 6) + 0] = (i * 4) + 0;
        indices[(i * 6) + 1] = (i * 4) + 1;
        indices[(i * 6) + 2] = (i * 4) + 2;
        indices[(i * 6) + 3] = (i * 4) + 0;
        indices[(i * 6) + 4] = (i * 4) + 2;
        indices[(i * 6) + 5] = (i * 4) + 3;
    }

    sg_buffer_desc ibuf_desc{};
    ibuf_desc.size = indices.size() * sizeof(std::uint16_t);
    ibuf_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
    ibuf_desc.content = std::data(indices);
    bindings.index_buffer = sg_make_buffer(ibuf_desc);

    sg_shader_desc shd_desc{};
    auto& ub = shd_desc.vs.uniform_blocks[0];
    ub.size = sizeof(vs_params_t);
    ub.uniforms[0].name = "screen_size";
    ub.uniforms[0].type = SG_UNIFORMTYPE_FLOAT2;
    ub.uniforms[1].name = "camera_position";
    ub.uniforms[1].type = SG_UNIFORMTYPE_FLOAT2;
    ub.uniforms[2].name = "camera_zoom";
    ub.uniforms[2].type = SG_UNIFORMTYPE_FLOAT;
    shd_desc.vs.source = vs_source;
    shd_desc.fs.source = fs_source;
    shd_desc.fs.images[0] = {"tex", SG_IMAGETYPE_2D};
    sg_shader shd = sg_make_shader(&shd_desc);

    sg_pipeline_desc pip_desc{};
    pip_desc.shader = shd;
    pip_desc.index_type = SG_INDEXTYPE_UINT16;
    pip_desc.layout.attrs[0] = {0, 0, SG_VERTEXFORMAT_FLOAT2};
    pip_desc.layout.attrs[1] = {0, 8, SG_VERTEXFORMAT_FLOAT2};
    pip_desc.layout.attrs[2] = {0, 16, SG_VERTEXFORMAT_FLOAT4};
    pip_desc.blend.enabled = true;
    pip_desc.blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    pip_desc.blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pip_desc.blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pip_desc.blend.dst_factor_alpha = SG_BLENDFACTOR_ONE;
    pipeline = sg_make_pipeline(&pip_desc);
}

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
    entt::view<entt::exclude_t<>, const map, tile_set> view) {
    for (auto&& [_, m, ts] : view.proxy()) {
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

void tile_plugin::update_tiles(entt::view<entt::exclude_t<>,
                                          const parent,
                                          const tile_chunk,
                                          const transform> view,
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
    for (auto&& [_, p, tc, tfm] : view.proxy()) {
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
        sg_apply_bindings(bindings);
        sg_draw(chunk.start_element, chunk.element_count, 1);
    }
}

} // namespace motor
