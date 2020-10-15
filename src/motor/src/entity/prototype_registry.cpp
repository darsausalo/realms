#include "motor/entity/prototype_registry.hpp"
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace motor {

void prototype_registry::transpire(const sol::table& defs) {
    defs.for_each([this](const auto& key, const auto& value) {
        if (key.is<std::string>() && value.is<sol::table>()) {
            auto def_name = key.as<std::string>();
            if (def_name.size() && def_name[0] == '@') {
                return;
            }
            auto def_id = entt::hashed_string::value(std::data(def_name));
            auto def_e = get_or_create(def_id);
            auto def = value.as<sol::table>();
            def.for_each([this, &def_e, &def_name](const auto& key,
                                                   const auto& value) {
                if (key.is<std::string>()) {
                    auto comp_name = key.as<std::string>();
                    if (comp_name == "tags" && value.is<sol::table>()) {
                        for (auto&& [_, tag] : value.as<sol::table>()) {
                            auto comp_id = entt::hashed_string::value(
                                std::data(tag.as<std::string>()));

                            if (components.is_defined(comp_id)) {
                                components.transpire(reg, def_e,
                                                     lua_input_archive{tag},
                                                     comp_id);
                            }
                        }
                        return;
                    }

                    auto comp_id =
                        comp_name == "tag" || comp_name == "tags"
                            ? entt::hashed_string::value(
                                  std::data(value.as<std::string>()))
                            : entt::hashed_string::value(std::data(comp_name));
                    if (!components.is_defined(comp_id)) {
                        spdlog::warn("component '{}' not defined", comp_name);
                        return;
                    }

                    components.transpire(reg, def_e, lua_input_archive{value},
                                         comp_id);
                }
            });
        }
    });
}

entt::entity prototype_registry::spawn(entt::registry& to,
                                       entt::id_type name_id) {
    auto src = get(name_id);
    if (src != entt::null) {
        auto dst = to.create();
        to.emplace<prototype>(dst, src);
        components.stamp(reg, src, to, dst);
        return dst;
    }
    return {entt::null};
}

entt::entity prototype_registry::get(entt::id_type name_id) const {
    if (auto it = prototypes.find(name_id); it != prototypes.cend()) {
        return it->second;
    }
    return {entt::null};
}

void prototype_registry::respawn(entt::registry& to) {
    to.view<prototype>().each([this, &to](entt::entity dst, auto& p) {
        auto src = p.value;
        if (reg.valid(src)) {
            components.patch(reg, src, to, dst);
        }
    });
}

entt::entity prototype_registry::get_or_create(entt::id_type name_id) {
    entt::entity e{entt::null};
    if (auto it = prototypes.find(name_id); it == prototypes.cend()) {
        e = reg.create();
        prototypes[name_id] = e;
    } else {
        e = it->second;
    }
    return e;
}

} // namespace motor

//==============================================================================
// TEST

#include <doctest/doctest.h>

namespace motor::test::prototype_registry {

enum class id : entt::id_type;

struct position {
    float x;
    float y;
};

template<typename Archive>
void serialize(Archive& ar, position& value) {
    ar.member(M(value.x));
    ar.member(M(value.y));
}

struct timer {
    int duration;
    int elapsed;
};

template<typename Archive>
void serialize(Archive& ar, timer& value) {
    ar.member(M(value.duration));
    ar.member(M(value.elapsed));
}

struct health {
    int max;
    int value;
};

template<typename Archive>
void serialize(Archive& ar, health& value) {
    ar.member(M(value.max));
    ar.member(M(value.value));
}

struct sprite {
    std::string ref;
};

template<typename Archive>
void serialize(Archive& ar, sprite& value) {
    ar(value.ref);
}

struct struct_sprite {
    std::string ref;
};

template<typename Archive>
void serialize(Archive& ar, struct_sprite& value) {
    ar.member(M(value.ref));
}

struct tag1 {};

static const char* script = R"(
defs = {
    entity1 = {
        id = 1,
        position = { x = 101, y = 201 },
        timer = { duration = 1001 },
        health = { max = 101 },
        -- just ignore unknown tag
        tags = {"enemy", "tag1"}
    },
    entity2 = {
        id = 2,
        position = { x = 102, y = 202 },
        health = { max = 102 },
        tag1 = {}
    },
    entity3 = {
        id = 3,
        position = { x = 103, y = 203 },
        timer = { duration = 1003 },
        tags = "enemy",
        sprite = "/assets/sprite_1.png"
    }
})";

static const char* script_v1 = R"(
defs = {
    entity1 = {
        id = 1,
        position = { x = 111, y = 201 },
        timer = { duration = 1001 },
        health = { max = 111 },
        -- just ignore unknown tag
        tags = {"enemy", "tag1"}
    },
    entity2 = {
        id = 2,
        position = { x = 112, y = 202 },
        health = { max = 112 },
        tag1 = {}
    },
    entity3 = {
        id = 3,
        position = { x = 113, y = 203 },
        timer = { duration = 1003 },
        tags = "enemy",
        sprite = "/assets/sprite_2.png"
    }
})";

static const char* bad_script = R"(
defs = {
    entity1 = {
        id = "bad id",
        position = "bad position",
        timer = { duration = "bad timer" },
        health = { max = { "bad health" } },
        tags = { { "bad tag" }, { "bad tag2" } }
    }
}
)";

} // namespace motor::test::prototype_registry

TEST_CASE("prototype_registry: transpire components") {
    using namespace motor::test::prototype_registry;

    motor::prototype_registry prototypes;

    prototypes.components.define<id>();
    prototypes.components.define<position>();
    prototypes.components.define<timer>();
    prototypes.components.define<health>();
    prototypes.components.define<sprite>();
    prototypes.components.define<tag1>();
    prototypes.components.define<"enemy"_hs>();

    {
        sol::state lua{};
        lua.script(script);
        sol::table defs = lua["defs"];
        prototypes.transpire(defs);
    }

    entt::registry reg{};

    auto e1 = prototypes.spawn(reg, "entity1"_hs);
    auto e2 = prototypes.spawn(reg, "entity2"_hs);
    auto e3 = prototypes.spawn(reg, "entity3"_hs);

    CHECK((e1 != entt::null));
    CHECK((e2 != entt::null));
    CHECK((e3 != entt::null));

    CHECK(static_cast<int32_t>(reg.get<id>(e1)) == 1);
    CHECK(reg.has<position>(e1));
    CHECK(reg.get<position>(e1).x == 101);
    CHECK(reg.get<position>(e1).y == 201);
    CHECK(reg.has<timer>(e1));
    CHECK(reg.get<timer>(e1).duration == 1001);
    CHECK(reg.has<health>(e1));
    CHECK(reg.get<health>(e1).max == 101);
    CHECK(reg.has<entt::tag<"enemy"_hs>>(e1));
    CHECK(reg.has<tag1>(e1));

    CHECK(static_cast<int32_t>(reg.get<id>(e2)) == 2);
    CHECK(reg.has<position>(e2));
    CHECK(reg.get<position>(e2).x == 102);
    CHECK(reg.get<position>(e2).y == 202);
    CHECK(reg.has<health>(e2));
    CHECK(reg.get<health>(e2).max == 102);
    CHECK(reg.has<tag1>(e2));

    CHECK(static_cast<int32_t>(reg.get<id>(e3)) == 3);
    CHECK(reg.has<position>(e3));
    CHECK(reg.get<position>(e3).x == 103);
    CHECK(reg.get<position>(e3).y == 203);
    CHECK(reg.has<timer>(e3));
    CHECK(reg.get<timer>(e3).duration == 1003);
    CHECK(reg.has<sprite>(e3));
    CHECK(reg.get<sprite>(e3).ref == "/assets/sprite_1.png");
    CHECK(reg.has<entt::tag<"enemy"_hs>>(e3));

    reg.remove<timer>(e3);
    CHECK(!reg.has<timer>(e3));

    {
        sol::state lua{};
        lua.script(script_v1);
        sol::table defs = lua["defs"];
        prototypes.transpire(defs);
    }
    prototypes.respawn(reg);

    CHECK(static_cast<int32_t>(reg.get<id>(e1)) == 1);
    CHECK(reg.has<position>(e1));
    CHECK(reg.get<position>(e1).x == 111);
    CHECK(reg.get<position>(e1).y == 201);
    CHECK(reg.has<timer>(e1));
    CHECK(reg.get<timer>(e1).duration == 1001);
    CHECK(reg.has<health>(e1));
    CHECK(reg.get<health>(e1).max == 111);
    CHECK(reg.has<entt::tag<"enemy"_hs>>(e1));
    CHECK(reg.has<tag1>(e1));

    CHECK(static_cast<int32_t>(reg.get<id>(e2)) == 2);
    CHECK(reg.has<position>(e2));
    CHECK(reg.get<position>(e2).x == 112);
    CHECK(reg.get<position>(e2).y == 202);
    CHECK(reg.has<health>(e2));
    CHECK(reg.get<health>(e2).max == 112);
    CHECK(reg.has<tag1>(e2));

    CHECK(static_cast<int32_t>(reg.get<id>(e3)) == 3);
    CHECK(reg.has<position>(e3));
    CHECK(reg.get<position>(e3).x == 113);
    CHECK(reg.get<position>(e3).y == 203);
    CHECK(!reg.has<timer>(e3));
    CHECK(reg.has<sprite>(e3));
    CHECK(reg.get<sprite>(e3).ref == "/assets/sprite_2.png");
    CHECK(reg.has<entt::tag<"enemy"_hs>>(e3));
}

TEST_CASE("prototype_registry: ignore bad values") {
    using namespace motor::test::prototype_registry;

    motor::prototype_registry prototypes;

    prototypes.components.define<id>();
    prototypes.components.define<position>();
    prototypes.components.define<timer>();
    prototypes.components.define<health>();
    prototypes.components.define<sprite>();
    prototypes.components.define<tag1>();
    prototypes.components.define<"enemy"_hs>();

    {
        sol::state lua{};
        lua.script(bad_script);
        sol::table defs = lua["defs"];
        prototypes.transpire(defs);
    }

    entt::registry reg{};

    auto e1 = prototypes.spawn(reg, "entity1"_hs);

    CHECK((e1 != entt::null));
    CHECK(static_cast<int32_t>(reg.get<id>(e1)) == 0);
    CHECK(reg.has<position>(e1));
    CHECK(reg.get<position>(e1).x == 0);
    CHECK(reg.get<position>(e1).y == 0);
    CHECK(reg.has<timer>(e1));
    CHECK(reg.get<timer>(e1).duration == 0);
    CHECK(reg.has<health>(e1));
    CHECK(reg.get<health>(e1).max == 0);
    CHECK(!reg.has<entt::tag<"enemy"_hs>>(e1));
    CHECK(!reg.has<tag1>(e1));
}
