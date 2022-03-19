#include "gui_plugin.hpp"
#include "app/window_plugin.hpp"
#include "motor/app/app_builder.hpp"
#include "motor/core/filesystem.hpp"
#include <algorithm>
#include <filesystem>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <nlohmann/json.hpp>

namespace motor {

void from_json(const nlohmann::json& j, gui_config& c) {
    j.at("default_font").get_to(c.default_font);
    j.at("default_font_size").get_to(c.default_font_size);
}

gui_plugin::gui_plugin(app_builder& app)
    : screen{app.registry().ctx<motor::screen>()}
    , config{} {
    try {
        app.registry().ctx<nlohmann::json>().at("gui").get_to(config);
    } catch (nlohmann::json::exception& e) {
        spdlog::warn("invalid gui config: {}", e.what());
    }

    auto fonts_path =
        filesystem::data() / "mods" / "core" / "resources" / "fonts";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    auto& wc = app.registry().ctx<window_context>();
    window = wc.window;

    ImGui_ImplSDL2_InitForOpenGL(wc.window, wc.gl_ctx);
    ImGui_ImplOpenGL3_Init("#version 130");

    if (!config.default_font.empty()) {
        auto default_font_path = fonts_path / config.default_font;
        if (std::filesystem::exists(default_font_path)) {
            ImVector<ImWchar> ranges;
            ImFontGlyphRangesBuilder builder;
            builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
            builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
            builder.BuildRanges(&ranges);
            io.Fonts->AddFontFromFileTTF(
                std::data(default_font_path.string()),
                std::max(std::size_t{8u}, config.default_font_size), nullptr,
                ranges.Data);
            io.Fonts->Build();
        } else {
            spdlog::warn("font '{}' not exists", default_font_path.string());
        }
    }

    app.add_system_to_stage<&gui_plugin::begin_frame>("pre_gui"_hs, *this)
        .add_system_to_stage<&gui_plugin::end_frame>("post_gui"_hs, *this);
}

gui_plugin::~gui_plugin() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

static bool show_demo_window = true;

void gui_plugin::begin_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
}

void gui_plugin::end_frame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace motor
