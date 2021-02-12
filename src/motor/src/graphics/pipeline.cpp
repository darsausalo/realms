#include "pipeline.hpp"

#include <vector>

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

} // namespace

void init_pipeline(sg_pipeline& pipeline, sg_bindings& bindings) {
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

} // namespace motor
