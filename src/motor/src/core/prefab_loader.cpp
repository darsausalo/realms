#include "motor/core/prefab_loader.h"

//==============================================================================
// TEST

#include <doctest/doctest.h>
#include <entt/entity/registry.hpp>
#include <fmt/core.h>
#include <functional>
#include <iostream>
#include <nameof.hpp>
#include <nlohmann/json.hpp>
#include <sstream>

namespace motor::test::prefab_loader {

enum class id : entt::id_type;

struct position {
    float x;
    float y;
};

struct timer {
    int duration;
    int elapsed;
};

struct health {
    int max;
    int value;
};

struct prefab_tag {};

static const char* json_text = R"({
    "entity1": {
        "motor::test::prefab_loader::id": 1,
        "motor::test::prefab_loader::position": { "x": 101, "y": 201 },
        "motor::test::prefab_loader::timer": { "duration": 1001 },
        "motor::test::prefab_loader::health": { "max": 101 },
        "motor::test::prefab_loader::prefab_tag": {}
    },
    "entity2": {
        "motor::test::prefab_loader::id": 2,
        "motor::test::prefab_loader::position": { "x": 102, "y": 202 },
        "motor::test::prefab_loader::health": { "max": 102 },
        "entt::tag<\"prefab_loader\"_hs>": {}
    },
    "entity3": {
        "motor::test::prefab_loader::id": 3,
        "motor::test::prefab_loader::position": { "x": 103, "y": 203 },
        "motor::test::prefab_loader::timer": { "duration": 1003 }
    }
})";

static nlohmann::json get_j(const nlohmann::json& j,
                            motor::test::prefab_loader::id id,
                            std::string_view name) {
    for (auto&& [_, value] : j.items()) {
        if (value["motor::test::prefab_loader::id"]
                    .get<motor::test::prefab_loader::id>() == id) {
            return value[std::data(name)];
        }
    }
    return {};
};

} // namespace motor::test::prefab_loader

REFL_AUTO(type(motor::test::prefab_loader::id));
REFL_AUTO(type(motor::test::prefab_loader::position), field(x), field(y));
REFL_AUTO(type(motor::test::prefab_loader::timer), field(duration),
          field(elapsed));
REFL_AUTO(type(motor::test::prefab_loader::health), field(max), field(value));
REFL_AUTO(type(motor::test::prefab_loader::prefab_tag));
REFL_AUTO(type(entt::tag<"prefab_loader"_hs>));

TEST_CASE("prefab_loader: load from json") {
    using namespace motor::test::prefab_loader;

    entt::registry reg;
    motor::prefab_loader loader{reg};

    nlohmann::json j = nlohmann::json::parse(json_text);

    loader.entities(j)
            .component<id, position, timer, health>()
            .component<prefab_tag, entt::tag<"prefab_loader"_hs>>();
    CHECK(reg.view<id, position>().size() == 3);
    CHECK(reg.view<position>().size() == 3);
    CHECK(reg.view<health>().size() == 2);
    CHECK(reg.view<timer>().size() == 2);
    CHECK(reg.view<prefab_tag>().size() == 1);
    CHECK(reg.view<entt::tag<"prefab_loader"_hs>>().size() == 1);

    reg.view<id, position>().each([&j](const auto& id, const auto& p) {
        auto jv = get_j(j, id, motor::nameof_type<position>());
        CHECK(p.x == jv["x"].get<float>());
        CHECK(p.y == jv["y"].get<float>());
    });
    reg.view<id, timer>().each([&j](const auto& id, const auto& h) {
        auto jv = get_j(j, id, motor::nameof_type<timer>());
        CHECK(h.duration == jv["duration"].get<int>());
    });
    reg.view<id, health>().each([&j](const auto& id, const auto& h) {
        auto jv = get_j(j, id, motor::nameof_type<health>());
        CHECK(h.max == jv["max"].get<int>());
    });
}
