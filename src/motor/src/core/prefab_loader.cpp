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

namespace motor {
template<>
struct type_name<entt::tag<"prefab"_hs>> : std::true_type {
    static constexpr char* name = "motor::test::prefab_loader::prefab";
};
} // namespace motor

namespace motor::test::prefab_loader {

// TODO: integral aliases?
// TODO: tags


struct position {
    float x;
    float y;
};

template<typename Archive>
void serialize(Archive& ar, position& p) {
    ar.member(M(p.x));
    ar.member(M(p.y));
}

struct timer {
    int duration;
    int elapsed;
};

template<typename Archive>
void serialize(Archive& ar, timer& t) {
    ar.member(M(t.duration));
    ar.member(M(t.elapsed));
}

struct health {
    int max;
    int value;
};

template<typename Archive>
void serialize(Archive& ar, health& h) {
    ar.member(M(h.max));
    ar.member(M(h.value));
}

struct prefab_tag {};

static const char* json_text = R"({
    "entity1": {
        "entt::id": 1,
        "motor::test::prefab_loader::position": { "x": 101, "y": 201 },
        "motor::test::prefab_loader::timer": { "duration": 1001 },
        "motor::test::prefab_loader::health": { "max": 101 },
        "motor::test::prefab_loader::prefab_tag": {}
    },
    "entity2": {
        "entt::id": 2,
        "motor::test::prefab_loader::position": { "x": 102, "y": 202 },
        "motor::test::prefab_loader::health": { "max": 102 },
        "motor::test::prefab_loader::prefab": {}
    },
    "entity3": {
        "entt::id": 3,
        "motor::test::prefab_loader::position": { "x": 103, "y": 203 },
        "motor::test::prefab_loader::timer": { "duration": 1003 }
    }
})";

static nlohmann::json get_j(const nlohmann::json& j, entt::id_type id,
                            std::string_view name) {
    for (auto&& [_, value] : j.items()) {
        if (value["entt::id"].get<entt::id_type>() == id) {
            return value[std::data(name)];
        }
    }
    return {};
};

} // namespace motor::test::prefab_loader

TEST_CASE("prefab_loader: load from json") {
    using namespace motor::test::prefab_loader;

    entt::registry reg;
    motor::prefab_loader loader{reg};

    nlohmann::json j = nlohmann::json::parse(json_text);

    loader.entities(j)
            .component<entt::id_type, position, timer, health>()
            .component<prefab_tag, entt::tag<"prefab"_hs>>();
    CHECK(reg.view<entt::id_type, position>().size() == 3);
    CHECK(reg.view<position>().size() == 3);
    CHECK(reg.view<health>().size() == 2);
    CHECK(reg.view<timer>().size() == 2);
    CHECK(reg.view<prefab_tag>().size() == 1);
    CHECK(reg.view<entt::tag<"prefab"_hs>>().size() == 1);

    reg.view<entt::id_type, position>().each(
            [&j](const auto& id, const auto& p) {
                auto jv = get_j(j, id, motor::nameof_type<position>());
                CHECK(p.x == jv["x"].get<float>());
                CHECK(p.y == jv["y"].get<float>());
            });
    reg.view<entt::id_type, timer>().each([&j](const auto& id, const auto& h) {
        auto jv = get_j(j, id, motor::nameof_type<timer>());
        CHECK(h.duration == jv["duration"].get<int>());
    });
    reg.view<entt::id_type, health>().each([&j](const auto& id, const auto& h) {
        auto jv = get_j(j, id, motor::nameof_type<health>());
        CHECK(h.max == jv["max"].get<int>());
    });
}
