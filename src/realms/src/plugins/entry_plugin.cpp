#include "entry_plugin.hpp"
#include <imgui.h>
#include <motor/app/app_builder.hpp>
#include <motor/core/input.hpp>
#include <motor/graphics/screen.hpp>
#include <entt/signal/dispatcher.hpp>
#include "events.hpp"

namespace realms {

using namespace entt::literals;

void quit_system(const motor::input_actions& input,
                 entt::dispatcher& dispatcher) {
    if (input.is_just_pressed("quit"_hs)) {
        dispatcher.trigger<motor::event::quit>();
    }
}

entry_plugin::entry_plugin(motor::app_builder& app)
    : screen{app.registry().ctx<motor::screen>()}
    , dispatcher{app.dispatcher()} {
    dispatcher.sink<event::start_entry>().connect<&entry_plugin::enter>(*this);
    dispatcher.sink<event::start_game>().connect<&entry_plugin::exit>(*this);

    app.add_system<&quit_system>()
        .add_system_to_stage<&entry_plugin::update_gui>("gui"_hs, *this);
}

void entry_plugin::enter() { active = true; }

void entry_plugin::exit() { active = false; }

void entry_plugin::update_gui() {
    static constexpr auto window_width = 220;
    static constexpr auto window_height = 160;
    static constexpr auto title_text = "Realms";
    static constexpr auto button_text = "Start";
    static constexpr auto quit_text = "Quit";

    if (!active) {
        return;
    }

    ImGui::SetNextWindowPos({(screen.width - window_width) * 0.5f,
                             (screen.height - window_height) * 0.5f});
    ImGui::SetNextWindowSize({window_width, window_height});

    ImGui::Begin(title_text, nullptr, ImGuiWindowFlags_NoDecoration);

    auto window_size = ImGui::GetWindowSize();
    auto text_size = ImGui::CalcTextSize(title_text);
    auto button_width = text_size.x;
    ImGui::SetCursorPos({(window_size.x - text_size.x) * 0.5f,
                         (window_size.y - text_size.y) * 0.5f - 20.0f});
    ImGui::Text(title_text);

    text_size = ImGui::CalcTextSize(button_text);
    auto button_height = text_size.y + 8.0f;
    ImGui::SetCursorPos({(window_size.x - button_width) * 0.5f,
                         (window_size.y - text_size.y) * 0.5f + 10.0f});
    if (ImGui::Button(button_text, {button_width, button_height})) {
        dispatcher.enqueue<event::start_game>();
    }

    text_size = ImGui::CalcTextSize(quit_text);
    ImGui::SetCursorPos({(window_size.x - button_width) * 0.5f,
                         (window_size.y - text_size.y) * 0.5f + 50.0f});
    if (ImGui::Button(quit_text, {button_width, button_height})) {
        dispatcher.enqueue<motor::event::quit>();
    }

    ImGui::End();
}

} // namespace realms
