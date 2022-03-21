#include "motor/entity/scheduler.hpp"

//==============================================================================
// TEST

#include <doctest/doctest.h>
#include <entt/core/hashed_string.hpp>

namespace {

using namespace entt::literals;

void ro_char_rw_int(entt::view<entt::get_t<int, const char>>) {}
void sync_point(entt::registry&) {}
void empty() {}

struct clazz {
    void rw_int(entt::view<entt::get_t<int>>) {}
};

} // namespace

TEST_CASE("scheduler: stage order") {
    motor::scheduler scheduler{};

    scheduler.add_stage("init"_hs);
    scheduler.add_stage("event"_hs);
    scheduler.add_stage("update"_hs);
    scheduler.add_stage_before("event"_hs, "pre_event"_hs);
    scheduler.add_stage_after("event"_hs, "post_event"_hs);
    scheduler.add_stage_before("update"_hs, "pre_update"_hs);
    scheduler.add_stage_after("update"_hs, "post_update"_hs);
    scheduler.add_stage("render"_hs);

    std::vector<entt::id_type> stages;
    scheduler.visit([&stages](auto stage) { stages.push_back(stage); });

    CHECK(stages[0] == "init"_hs);
    CHECK(stages[1] == "pre_event"_hs);
    CHECK(stages[2] == "event"_hs);
    CHECK(stages[3] == "post_event"_hs);
    CHECK(stages[4] == "pre_update"_hs);
    CHECK(stages[5] == "update"_hs);
    CHECK(stages[6] == "post_update"_hs);
    CHECK(stages[7] == "render"_hs);
}

TEST_CASE("scheduler: systems") {
    motor::scheduler scheduler{};
    clazz instance{};

    scheduler.add_stage("stage1"_hs);
    scheduler.add_stage("stage2"_hs);

    scheduler.add_system_to_stage<&ro_char_rw_int>(
        "stage1"_hs, "default"_hs, "t1");
    scheduler.add_system_to_stage<&clazz::rw_int>(
        "stage1"_hs, instance, "entry"_hs, "t2");
    scheduler.add_system_to_stage<&sync_point>(
        "stage1"_hs, "game"_hs, "sync_point");
    scheduler.add_system_to_stage<&empty>("stage1"_hs, "pause"_hs, "empty");
    scheduler.add_system_to_stage<&empty>("stage2"_hs, "pause"_hs, "empty");

    std::vector<std::pair<std::string, entt::id_type>> systems;

    const auto graph = scheduler.graph("stage1"_hs);

    for (auto&& [v, l] : graph) {
        systems.emplace_back(v.name(), l);
    }

    CHECK(systems[0].first == "t1");
    CHECK(systems[0].second == "default"_hs);
    CHECK(systems[1].first == "t2");
    CHECK(systems[1].second == "entry"_hs);
    CHECK(systems[2].first == "sync_point");
    CHECK(systems[2].second == "game"_hs);
    CHECK(systems[3].first == "empty");
    CHECK(systems[3].second == "pause"_hs);

    systems.clear();
    const auto graph2 = scheduler.graph("stage2"_hs);
    for (auto&& [v, l] : graph2) {
        systems.emplace_back(v.name(), l);
    }
    CHECK(systems[0].first == "empty");
    CHECK(systems[0].second == "pause"_hs);
}
