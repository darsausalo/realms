#include "motor/entity/scheduler.hpp"

//==============================================================================
// TEST

#include <doctest/doctest.h>
#include <entt/core/hashed_string.hpp>

namespace {

void ro_char_rw_int(entt::view<entt::exclude_t<>, int, const char>) {
}

void sync_point(entt::registry&) {
}

void empty() {
}

struct clazz {
    void rw_int(entt::view<entt::exclude_t<>, int>) {}
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

    scheduler.add_system_to_stage<&ro_char_rw_int>("stage1"_hs, "t1");
    scheduler.add_system_to_stage<&clazz::rw_int>("stage1"_hs, instance, "t2");
    scheduler.add_system_to_stage<&sync_point>("stage1"_hs, "sync_point");
    scheduler.add_system_to_stage<&empty>("stage1"_hs, "empty");

    std::vector<std::string> systems;

    const auto graph = scheduler.graph("stage1"_hs);

    for (auto&& v : graph) {
        systems.push_back(v.name());
    }

    CHECK(systems[0] == "t1");
    CHECK(systems[1] == "t2");
    CHECK(systems[2] == "sync_point");
    CHECK(systems[3] == "empty");
}
