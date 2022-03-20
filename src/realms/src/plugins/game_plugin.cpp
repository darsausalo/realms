#include "game_plugin.hpp"
#include "components/registration.hpp"
#include "events.hpp"
#include <entt/signal/dispatcher.hpp>
#include <imgui.h>
#include <motor/app/app_builder.hpp>
#include <motor/core/input.hpp>
#include <motor/graphics/screen.hpp>

namespace realms {

using namespace entt::literals;

game_plugin::game_plugin(motor::app_builder& app)
    : registry{app.registry()}
    , screen{app.registry().ctx<motor::screen>()}
    , dispatcher{app.dispatcher()} {
    register_components(app);

    dispatcher.sink<event::start_game>().connect<&game_plugin::enter>(*this);
    dispatcher.sink<event::start_entry>().connect<&game_plugin::exit>(*this);

    app.add_system_to_stage<&game_plugin::update_gui>("gui"_hs, *this);
}

void game_plugin::enter() { active = true; }

void game_plugin::exit() { active = false; }

void game_plugin::update_gui() {
    static constexpr auto window_width = 250;
    static constexpr auto window_height = 110;
    static constexpr auto title_text = "Work In Progress";
    static constexpr auto button_text = "Stop";

    if (!active) {
        return;
    }

    ImGui::SetNextWindowPos({(screen.width - window_width) * 0.5f,
                             (screen.height - window_height) * 0.5f});
    ImGui::SetNextWindowSize({window_width, window_height});

    ImGui::Begin(title_text, nullptr, ImGuiWindowFlags_NoDecoration);

    auto window_size = ImGui::GetWindowSize();
    auto text_size = ImGui::CalcTextSize(title_text);
    ImGui::SetCursorPos({(window_size.x - text_size.x) * 0.5f,
                         (window_size.y - text_size.y) * 0.5f - 20.0f});
    ImGui::Text(title_text);

    text_size = ImGui::CalcTextSize(button_text);
    ImGui::SetCursorPos({(window_size.x - text_size.x) * 0.5f,
                         (window_size.y - text_size.y) * 0.5f + 10.0f});
    if (ImGui::Button(button_text)) {
        dispatcher.enqueue<event::start_entry>();
    }

    ImGui::End();
}

} // namespace realms
