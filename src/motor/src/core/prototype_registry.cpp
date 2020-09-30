#include "motor/core/prototype_registry.h"
#include "motor/services/components_service.h"
#include "motor/services/locator.h"
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace motor {

prototype_registry::prototype_registry(const sol::table& defs) {
    defs.for_each([this](const auto& key, const auto& value) {
        if (key.is<std::string>() && value.is<sol::table>()) {
            auto def_name = key.as<std::string>();
            if (def_name.size() && def_name[0] == '@') {
                return;
            }
            auto def_id = entt::hashed_string::value(std::data(def_name));
            auto def_e = get_or_create(def_id);
            auto def = value.as<sol::table>();
            spdlog::debug("created {}({})", def_name, def_id);
            def.for_each([this, &def_e, &def_name](const auto& key,
                                                   const auto& value) {
                if (key.is<std::string>()) {
                    auto comp_name = key.as<std::string>();
                    if (comp_name == "tags" && value.is<sol::table>()) {
                        for (auto&& [_, tag] : value.as<sol::table>()) {
                            auto comp_id = entt::hashed_string::value(
                                    std::data(tag.as<std::string>()));

                            if (locator::components::ref().exists(comp_id)) {
                                locator::components::ref().transpire(
                                        reg, def_e, lua_input_archive{tag},
                                        comp_id);
                            }
                        }
                        return;
                    }

                    auto comp_id =
                            comp_name == "tag" || comp_name == "tags"
                                    ? entt::hashed_string::value(std::data(
                                              value.as<std::string>()))
                                    : entt::hashed_string::value(
                                              std::data(comp_name));
                    if (!locator::components::ref().exists(comp_id)) {
                        return;
                    }

                    spdlog::debug("transpire {}.{}", def_name, comp_name);

                    locator::components::ref().transpire(
                            reg, def_e, lua_input_archive{value}, comp_id);
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
        locator::components::ref().stamp(reg, src, to, dst);
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

struct tag1 {};

static const char* script = R"(
defs = {
    entity1 = {
        id = 1,
        position = { x = 101, y = 201 },
        timer = { duration = 1001 },
        health = { max = 101 },
        tags = {"enemy", "ingored_tag", "tag1"}
    },
    entity2 = {
        id = 2,
        position = { x = 102, y = 202 },
        health = { max = 102 },
        tag1 = {},
        ingored_comp = {}
    },
    entity3 = {
        id = 3,
        position = { x = 103, y = 203 },
        timer = { duration = 1003 },
        tags = "enemy"
    }
})";

} // namespace motor::test::prototype_registry

TEST_CASE("prototype_registry: transpire components") {
    using namespace motor::test::prototype_registry;

    motor::locator::components::set<motor::components_service>();

    motor::locator::components::ref()
            .component<id, position, timer, health, tag1>();
    motor::locator::components::ref().tag<"enemy"_hs>();

    sol::state lua{};
    lua.script(script);

    sol::table tbl = lua["defs"];

    motor::prototype_registry prototypes{tbl};

    entt::registry reg{};

    auto e1 = prototypes.spawn(reg, "entity1"_hs);
    auto e2 = prototypes.spawn(reg, "entity2"_hs);
    auto e3 = prototypes.spawn(reg, "entity3"_hs);

    CHECK((e1 != entt::null));
    CHECK((e2 != entt::null));
    CHECK((e3 != entt::null));

    CHECK(static_cast<int32_t>(reg.get<id>(e1)) == 1);
    CHECK(reg.get<position>(e1).x == 101);
    CHECK(reg.get<position>(e1).y == 201);
    CHECK(reg.get<timer>(e1).duration == 1001);
    CHECK(reg.get<health>(e1).max == 101);

    CHECK(static_cast<int32_t>(reg.get<id>(e2)) == 2);
    CHECK(reg.get<position>(e2).x == 102);
    CHECK(reg.get<position>(e2).y == 202);
    CHECK(reg.get<health>(e2).max == 102);

    CHECK(static_cast<int32_t>(reg.get<id>(e3)) == 3);
    CHECK(reg.get<position>(e3).x == 103);
    CHECK(reg.get<position>(e3).y == 203);
    CHECK(reg.get<timer>(e3).duration == 1003);

    CHECK(reg.view<tag1>().size() == 2);
    CHECK(reg.view<entt::tag<"enemy"_hs>>().size() == 2);

    motor::locator::components::reset();
}

// TODO: bad scenario tests
