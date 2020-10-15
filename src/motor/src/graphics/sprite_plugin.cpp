#include "sprite_plugin.hpp"
#include "motor/app/app_builder.hpp"
#include <algorithm>
#include <array>
#include <entt/entity/registry.hpp>

namespace motor {

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
    vec2 result_position = ((position - camera_position) * camera_zoom) + 0.5;
    gl_Position = vec4(((result_position / screen_size)) * vec2(2.0, 2.0), 0.0, 1.0);

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

static constexpr const std::size_t max_batch_size = 1024 * 10;
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

sprite_plugin::sprite_plugin(app_builder& app) {
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

    app.define_component<sprite>()
        .define_component<sprite_sheet>()
        .add_system_to_stage<&sprite_plugin::update_sprite_sheets>(
            "post_update"_hs, *this)
        .add_system_to_stage<&sprite_plugin::prepare_sprites>("pre_render"_hs,
                                                              *this)
        .add_system_to_stage<&sprite_plugin::prepare_sprite_sheets>(
            "pre_render"_hs, *this)
        .add_system_to_stage<&sprite_plugin::emplace_sprites>("pre_render"_hs,
                                                              *this)
        .add_system_to_stage<&sprite_plugin::render_sprites>("render"_hs,
                                                             *this);

    app.registry().set<camera2d>(glm::vec2{0.0f, 0.0f}, 2.0f);
}

void sprite_plugin::prepare_sprites(
    entt::view<entt::exclude_t<sg_image>, sprite> view,
    entt::registry& registry) {
    view.each([&registry](auto e, auto& s) {
        registry.emplace<sg_image>(e, s.image->upload());
        registry.emplace<rect>(e, glm::vec2{0.0f, 0.0f}, s.image->size());
        registry.emplace<image_atlas>(e, s.image->size());
    });
}

void sprite_plugin::prepare_sprite_sheets(
    entt::view<entt::exclude_t<sg_image>, sprite_sheet> view,
    entt::registry& registry) {
    view.each([&registry](auto e, auto& s) {
        registry.emplace<sg_image>(e, s.image->upload());
        registry.emplace<rect>(e,
                               glm::vec2{0.0f, 0.0f},
                               s.image->size() / glm::vec2{s.columns, s.rows});
        registry.emplace<image_atlas>(e, s.image->size());
    });
}

void sprite_plugin::update_sprite_sheets(
    entt::view<entt::exclude_t<>, const sprite_sheet, rect> view) {
    view.each([](auto e, auto& s, auto& r) {
        auto column = s.index % s.columns;
        auto row = (s.index / s.columns) % s.rows;
        auto size = s.image->size() / glm::vec2{s.columns, s.rows};
        r.min = glm::vec2{column, row} * size;
        r.max = r.min + size;
    });
}

void sprite_plugin::emplace_sprites(
    entt::view<entt::exclude_t<entt::tag<"hidden"_hs>>,
               const sg_image,
               const transform,
               const rect,
               const image_atlas> view) {
    sprites.clear();
    view.each(
        [this](
            const auto& image, const auto& tfm, const auto& r, const auto& ia) {
            sprites.push_back({image,
                               tfm.value,
                               {
                                   glm::vec2{r.min.x, r.max.y}, // top left
                                   glm::vec2{r.max.x, r.max.y}, // top right
                                   glm::vec2{r.max.x, r.min.y}, // bottom right
                                   glm::vec2{r.min.x, r.min.y}, // bottom left
                               },
                               ia.size});
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
                          camera.position,
                          std::max(camera.zoom, 1.0f)};

    sg_apply_pipeline(pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &vs_params, sizeof(vs_params_t));

    sg_image batch_image{};
    std::size_t batch_start{};
    for (std::size_t i = 0u; i < sprites.size(); ++i) {
        sg_image image = sprites[i].image;
        if (image.id != batch_image.id) {
            if (i > batch_start) {
                render_batch(
                    batch_image, &sprites[batch_start], i - batch_start);
            }

            batch_image = image;
            batch_start = i;
        }
    }

    render_batch(
        batch_image, &sprites[batch_start], sprites.size() - batch_start);
}

// TODO: hack to sokol
void sg_rewind_buffer(sg_buffer buf_id);

void sprite_plugin::render_batch(sg_image image,
                                 const sprite_data* sprites,
                                 std::size_t count) {
    // clang-format off
    static std::array<glm::vec2, 4> corners = {
        glm::vec2{-0.5f,  0.5f},
        glm::vec2{ 0.5f,  0.5f},
        glm::vec2{ 0.5f, -0.5f},
        glm::vec2{-0.5f, -0.5f},
    };
    // clang-format on
    std::array<vertex, 4> vertices;
    while (count > 0) {
        auto batch_size = std::min(count, max_batch_size);
        for (std::size_t i = 0u; i < batch_size; ++i) {
            auto& sprite = sprites[i];
            auto size = (sprite.rect[1] - sprite.rect[3]);
            for (std::size_t j = 0u; j < sprite.rect.size(); ++j) {
                vertices[j].position =
                    sprite.tfm * glm::vec4(corners[j] * size, 0.0f, 1.0f);
                vertices[j].uv = sprite.rect[j] / sprite.atlas_size;
                vertices[j].color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
            }
            sg_append_buffer(bindings.vertex_buffers[0],
                             &vertices[0],
                             vertices.size() * sizeof(vertex));
        }
        bindings.fs_images[0] = image;
        sg_apply_bindings(bindings);
        sg_draw(0, count * 6, 1); // TODO: rewind or overflow?

        // TODO: hack to sokol
        sg_rewind_buffer(bindings.vertex_buffers[0]);

        sprites += batch_size;
        count -= batch_size;
    }
}

} // namespace motor
