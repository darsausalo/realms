#include "gui_plugin.hpp"
#include "app/window_plugin.hpp"
#include "motor/app/app_builder.hpp"
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

namespace motor {

static constexpr char* glsl_version = "#version 130";

gui_plugin::gui_plugin(app_builder& app)
    : screen{app.registry().ctx<motor::screen>()} {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    auto& wc = app.registry().ctx<window_context>();
    window = wc.window;

    ImGui_ImplSDL2_InitForOpenGL(wc.window, wc.gl_ctx);
    ImGui_ImplOpenGL3_Init(glsl_version);

    app.add_system_to_stage<&gui_plugin::draw>("render"_hs, *this);
}

gui_plugin::~gui_plugin() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

static bool show_demo_window = true;

void gui_plugin::draw() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace motor
