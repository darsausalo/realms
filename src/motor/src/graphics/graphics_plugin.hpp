#ifndef MOTOR_GRAPHICS_PLUGIN_HPP
#define MOTOR_GRAPHICS_PLUGIN_HPP

#include "motor/graphics/screen.hpp"
#include <entt/entity/fwd.hpp>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vector>

namespace motor {

class app_builder;
class image_atlas;

struct clear_color {
    float r;
    float g;
    float b;
    float a;
};

struct graphics_config {
    clear_color clear_color{0.0f, 0.0f, 0.0f, 1.0f};
    std::uint32_t max_texture_size{1024};
};

class graphics_plugin {
public:
    graphics_plugin(app_builder& app);
    ~graphics_plugin();

private:
    screen& screen;
    nlohmann::json& jconfig;
    graphics_config config;

    std::vector<std::shared_ptr<image_atlas>> atlases{};

    void pre_render();
    void post_render();

    void build_atlases();
    void prepare_atlases();
    void dump_atlases(const entt::registry& registry);
};

} // namespace motor

#endif // MOTOR_GRAPHICS_PLUGIN_HPP
