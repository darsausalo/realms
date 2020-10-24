#include "motor/core/lua_archive.hpp"

//==============================================================================
// TEST

#include <doctest/doctest.h>
#include <entt/entity/registry.hpp>

namespace motor::test::lua_archive {

struct bullet {
    float damage;
};

template<typename Archive>
void serialize(Archive& ar, bullet& b) {
    ar.member(M(b.damage));
}

struct weapon {
    float damage;
    float crit;
    std::array<bullet, 3> bullets;
};

template<typename Archive>
void serialize(Archive& ar, weapon& w) {
    ar.member(M(w.damage));
    ar.member(M(w.crit));
    ar.member(M(w.bullets));
}

struct monster {
    std::string name;
    int max_health;
    weapon weapon;
    std::array<int, 3> inventory;
};

template<typename Archive>
void serialize(Archive& ar, monster& m) {
    ar.member(M(m.max_health));
    ar.member(M(m.weapon));
    ar.member(M(m.inventory));
}

static const char* script = R"(
monster = {
    name = "dark eye",
    weapon = {
        crit = 0.5,
        damage = 0.1,
        bullets = {{damage = 0.5}, {damage = 0.4}}
    },
    inventory = {1, nil, 6},
    max_health = 100
}
)";

static const char* simple_script = R"(
weapon = {
    crit = 0.5,
    damage = 0.1,
    bullets = {{damage = 0.5}, {damage = 0.4}}
}
)";

} // namespace motor::test::lua_archive

TEST_CASE("lua table deserialization: simple") {
    using namespace motor::test::lua_archive;

    sol::state lua;
    lua.script(simple_script);

    sol::table tbl = lua["weapon"];

    weapon weapon1{};
    {
        motor::lua_input_archive input{tbl};
        serialize(input, weapon1);
    }
    CHECK(weapon1.crit == 0.5f);
    CHECK(weapon1.damage == 0.1f);
    CHECK(weapon1.bullets[0].damage == 0.5f);
    CHECK(weapon1.bullets[1].damage == 0.4f);
}

TEST_CASE("lua table deserialization: struct") {
    using namespace motor::test::lua_archive;

    sol::state lua;
    lua.script(script);

    sol::table tbl = lua["monster"];

    monster monster1{};
    {
        motor::lua_input_archive input{tbl};
        serialize(input, monster1);
    }
    CHECK(monster1.max_health == 100);
    CHECK(monster1.weapon.crit == 0.5f);
    CHECK(monster1.weapon.damage == 0.1f);
    CHECK(monster1.weapon.bullets[0].damage == 0.5f);
    CHECK(monster1.weapon.bullets[1].damage == 0.4f);
    CHECK(monster1.inventory[0] == 1);
    CHECK(monster1.inventory[1] == 0);
    CHECK(monster1.inventory[2] == 6);
}

TEST_CASE("lua table deserialization: vector") {
    sol::state lua;
    lua.script("my_arr = {'val1', 'val2', 'val3'}");

    sol::table tbl = lua["my_arr"];

    std::vector<std::string> v;
    {
        motor::lua_input_archive input{tbl};
        serialize(input, v);
    }
    CHECK(v.size() == 3);
    CHECK(v[0] == "val1");
    CHECK(v[1] == "val2");
    CHECK(v[2] == "val3");
}
