#include "binary.h"
#include "plugin_context.h"
#include "prefab_loader.h"
#include "system_module.h"
#include <entt/entity/snapshot.hpp>
#include <functional>

static const char* json_text = R"({
    "entity1": {
        "position": { "x": 101, "y": 201 },
        "timer": { "duration": 1001 },
        "health": { "max": 101 }
    },
    "entity2": {
        "position": { "x": 102, "y": 202 },
        "health": { "max": 102 }
    },
    "entity3": {
        "position": { "x": 103, "y": 203 },
        "timer": { "duration": 1003 }
    }
})";

namespace entt {

template<typename Archive>
void serialize(Archive& ar, entity& e) {
    ar(static_cast<id_type>(e));
}

} // namespace entt

struct position {
    float x;
    float y;
};

template<typename Archive>
void serialize(Archive& ar, position& p) {
    ar.member("x", p.x);
    ar.member("y", p.y);
}

struct timer {
    int duration;
    int elapsed;
};

template<typename Archive>
void serialize(Archive& ar, timer& t) {
    ar.member("duration", t.duration);
    ar.member("elapsed", t.elapsed);
}

struct health {
    int max;
    int value;
};

template<typename Archive>
void serialize(Archive& ar, health& h) {
    ar.member("max", h.max);
    ar.member("value", h.value);
}

class module_a : public motor::system_module {
public:
    module_a() {
        component<position>();
        component<timer>();
        component<health>();
    }
};

extern "C" void plugin_entry(motor::plugin_context* ctx) {
    ctx->register_module<module_a>();
}

int main(int argc, char const* argv[]) {

    entt::registry reg;
    motor::prefab_loader loader{reg};

    nlohmann::json j = nlohmann::json::parse(json_text);

    motor::plugin_context ctx;

    ctx.register_module<module_a>();

    std::cout << "go!" << std::endl;

    ctx.load_prefabs(loader.entities(j));

    reg.view<position>().each([](const auto& p) {
        std::cout << "p = " << p.x << "," << p.y << std::endl;
    });

    reg.view<timer>().each([](const auto& t) {
        std::cout << "t = " << t.duration << "," << t.elapsed << std::endl;
    });

    reg.view<health>().each([](const auto& h) {
        std::cout << "h = " << h.max << "," << h.value << std::endl;
    });

    return 0;
}
