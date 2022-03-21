#include "components/base.hpp"
#include "events.hpp"
#include <fmt/format.h>
#include <imgui.h>
#include <motor/app/app_builder.hpp>
#include <motor/graphics/screen.hpp>
#include <string>

namespace realms {

using namespace entt::literals;

struct hud_context {
    std::string info{};
};

void show_hud(
    entt::view<entt::get_t<const position, entt::tag<"player"_hs>>> view,
    const motor::screen& screen,
    hud_context& hud,
    entt::dispatcher& dispatcher) {

    static constexpr auto* hud_window_name = "HUD";
    static constexpr auto hud_window_flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav;
    static constexpr auto hud_window_width = 250.0f;
    static constexpr auto hud_window_height = 110.0f;

    // update data

    hud.info.clear();
    if (auto it = view.begin(); it != view.end()) {
        const auto& p = view.get<const position>(*it);

        fmt::format_to(
            std::back_inserter(hud.info), "POS: {:.0f},{:.0f}", p.x, p.y);
    }

    // draw hud

    ImGui::SetNextWindowPos({(screen.width - hud_window_width) * 0.5f,
                             (screen.height - hud_window_height)});
    ImGui::SetNextWindowSize({hud_window_width, hud_window_height});
    ImGui::Begin(hud_window_name, nullptr, hud_window_flags);
    {
        auto window_size = ImGui::GetWindowSize();
        auto text_size = ImGui::CalcTextSize(std::data(hud.info));
        ImGui::SetCursorPos({(window_size.x - text_size.x) * 0.5f,
                             (window_size.y - text_size.y) * 0.5f - 20.0f});
        ImGui::Text(std::data(hud.info));

        text_size = ImGui::CalcTextSize("Stop");
        ImGui::SetCursorPos({(window_size.x - text_size.x) * 0.5f,
                             (window_size.y - text_size.y) * 0.5f + 10.0f});
        if (ImGui::Button("Stop")) {
            dispatcher.enqueue<event::start_entry>();
        }
    }
    ImGui::End();
}

void register_hud(motor::app_builder& app) {
    app.registry().set<hud_context>();

    app.add_system_to_stage<&show_hud>("gui"_hs, "game"_hs);
}

} // namespace realms
