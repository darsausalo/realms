#include "entry_plugin.hpp"
#include "events.hpp"
#include <entt/signal/dispatcher.hpp>
#include <imgui.h>
#include <motor/app/app_builder.hpp>
#include <motor/core/input.hpp>
#include <motor/graphics/screen.hpp>

namespace realms {

using namespace entt::literals;

static void quit_system(const motor::input_actions& input,
                        entt::dispatcher& dispatcher) {
    if (input.is_just_pressed("quit"_hs)) {
        dispatcher.trigger<motor::event::quit>();
    }
}

entry_plugin::entry_plugin(motor::app_builder& app)
    : registry{app.registry()}
    , game{app.registry().set<game_context>("entry"_hs)}
    , dispatcher{app.dispatcher()}
    , screen{app.registry().ctx<motor::screen>()} {
    dispatcher.sink<event::start_entry>().connect<&entry_plugin::enter>(*this);

    app.add_system<&quit_system>()
        .add_system_to_stage<&entry_plugin::update_gui>("gui"_hs, *this);
}

void entry_plugin::enter() {
    game.state = "entry"_hs;

    registry.clear();
}

void entry_plugin::update_gui() {
    if (game.state != "entry"_hs) {
        return;
    }

    static constexpr auto window_width = 220;
    static constexpr auto window_height = 160;
    static constexpr auto title_text = "Realms";
    static constexpr auto button_text = "Start";
    static constexpr auto quit_text = "Quit";

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
