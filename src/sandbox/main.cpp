#include "../motor/src/platform/backward.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <chrono>
#include <entt/entt.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string_view>
#include <unordered_map>

using namespace std::string_view_literals;

namespace backward {

SignalHandling sh;

std::string report_path;

} // namespace backward

struct position {
    float x;
    float y;
};

struct timer {
    int duration;
    int elapsed{0};
};

struct relationship {
    entt::entity parent;
};

struct name {
    std::string value;
};

void from_json(const nlohmann::json& j, position& p) {
    p.x = j.contains("x") ? j["x"].get<float>() : 0.0f;
    p.y = j.contains("y") ? j["y"].get<float>() : 0.0f;
}

template<typename Archvie>
void serialize(Archvie& ar, position& p) {
    ar(CEREAL_NVP(p.x), CEREAL_NVP(p.y));
}

void from_json(const nlohmann::json& j, timer& t) {
    t.duration = j.contains("duration") ? j["duration"].get<int>() : 0;
    t.elapsed = j.contains("elapsed") ? j["elapsed"].get<int>() : 0;
}

template<typename Archvie>
void serialize(Archvie& ar, timer& t) {
    ar(CEREAL_NVP(t.duration), CEREAL_NVP(t.elapsed));
};

void from_json(const nlohmann::json& j, relationship& r) {
}

template<typename Archvie>
void serialize(Archvie& ar, relationship& r) {
    ar(CEREAL_NVP(r.parent));
};

void from_json(const nlohmann::json& j, name& n) {
    n.value = j.contains("value") ? j["value"] : "";
}

template<typename Archvie>
void serialize(Archvie& ar, name& n) {
    ar(cereal::make_nvp("name", n.value));
};

static const char* def_input = R"({
    "entity1": {
        "name": { "value" : "test1" },
        "position": { "x": 4.0, "y": 1 },
        "timer": { "duration": 2000, "elapsed": 150 }
    },
    "entity2": {
        "name": { "value" : "test2" },
        "position": { "x": 5.0, "y": 8 },
        "timer": { "duration": 2111 },
        "relationship": {
            "parent": "entity1"
        }
    },
    "entity3": {
        "name": { "value" : "test3" },
        "position": { "x": 15.0, "y": 18 },
        "timer": { "elapsed": 321 }
    }
})";

struct comp_loader_base {
    virtual ~comp_loader_base() = default;

    virtual void load(const nlohmann::json& j, entt::registry& reg,
                      entt::entity e) = 0;
};

template<typename Type>
struct comp_loader : comp_loader_base {
    void load(const nlohmann::json& j, entt::registry& reg,
              entt::entity e) override {
        Type instance{};
        from_json(j, instance);
        reg.template emplace<Type>(e, instance);
    }
};

static std::unordered_map<std::string_view, std::unique_ptr<comp_loader_base>>
        comp_map;

class my_defs {
public:
    my_defs(nlohmann::json& in_j) : j(&in_j) {}

    void load(entt::registry& reg) {
        for (auto& el : j->items()) {
            std::cout << el.key() << ":" << el.value() << "\n";
            auto e = reg.create();
            for (auto& cel : el.value().items()) {
                if (auto it = comp_map.find(cel.key()); it != comp_map.end()) {
                    it->second->load(cel.value(), reg, e);
                }
            }
        }
    }

private:
    nlohmann::json* j;
};

int main(int argc, char const* argv[]) {
    std::cout << "Serialization playground:\n";

    std::cout << "defs:\n";

    nlohmann::json j = nlohmann::json::parse(def_input);

    entt::registry source;
    entt::registry destination;

    auto* l1 = new comp_loader<position>();
    comp_map.insert_or_assign("position"sv,
                              std::unique_ptr<comp_loader_base>(l1));
    comp_map.insert_or_assign("timer"sv,
                              std::make_unique<comp_loader<timer>>());
    comp_map.insert_or_assign("relationship"sv,
                              std::make_unique<comp_loader<relationship>>());
    comp_map.insert_or_assign("name"sv, std::make_unique<comp_loader<name>>());

    my_defs defs{j};
    defs.load(source);

    auto e0 = source.create();
    source.emplace<name>(e0, "e0");
    source.emplace<position>(e0, 2.f, 4.f);

    auto e1 = source.create();
    source.emplace<name>(e1, "e1");
    source.emplace<position>(e1, 12.f, 3.f);
    source.emplace<timer>(e1, 1000, 100);
    source.emplace<relationship>(e1, e0);

    auto e2 = source.create();
    source.emplace<name>(e2, "e2");

    auto e3 = source.create();
    source.emplace<name>(e3, "e3");
    source.emplace<timer>(e3, 300, 50);
    source.emplace<entt::tag<"empty"_hs>>(e3);

    std::cout << "serialize =>\n";
    std::stringstream s;

    {
        position p{2.0f, 3.0f};
        cereal::JSONOutputArchive output{s};
        entt::snapshot snapshot{source};
        snapshot.entities(output)
                .component<position, timer, relationship>(output)
                .component<entt::tag<"empty"_hs>>(output)
                .component<name>(output);

        std::ofstream fs("reg.bin", std::ios_base::binary);
        cereal::BinaryOutputArchive outbin{fs};
        snapshot.entities(outbin)
                .component<position, timer, relationship, entt::tag<"empty"_hs>,
                           name>(outbin);
    }
    std::cout << s.str() << "\n\n";

    std::cout << "deserialize =>\n";
    position p;
    cereal::JSONInputArchive input{s};
    entt::snapshot_loader loader{destination};
    loader.entities(input)
            .component<position, timer, relationship, entt::tag<"empty"_hs>>(
                    input)
            .component<name>(input);

    std::cout << "positions:\n";
    destination.view<const position>().each(
            [](const auto& p) { std::cout << p.x << ", " << p.y << "\n"; });
    std::cout << "timers:\n";
    destination.view<const timer>().each([](const auto& t) {
        std::cout << t.duration << ", " << t.elapsed << "\n";
    });
    std::cout << "relationships:\n";
    destination.view<const relationship>().each([](const auto& r) {
        std::cout << static_cast<std::uint32_t>(r.parent) << "\n";
    });

    /*auto start = std::chrono::system_clock::now();
    decltype(start) end;
    {
        constexpr std::size_t count = 100000;

        destination.clear();
        destination.reserve(count);
        destination.reserve<position>(count);
        destination.reserve<timer>(count);
        destination.reserve<name>(count);
        for (auto i = 0u; i < count; i++) {
            auto e = destination.create();
            destination.emplace<position>(e, 1.0f, 2.0f);
            destination.emplace<timer>(e, 1000, 100);
            destination.emplace<name>(e, "test");
        }

        end = std::chrono::system_clock::now();

        std::ofstream fs("reg2.bin", std::ios_base::binary);
        cereal::BinaryOutputArchive outbin{fs};
        entt::snapshot snapshot{destination};
        snapshot.entities(outbin)
                .component<position, timer, relationship, entt::tag<"empty"_hs>,
                           name>(outbin);
    }
    auto end2 = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end2 - end;
    std::chrono::duration<double> diff2 = end - start;
    std::cout << "create time: " << diff2.count() << "\n";
    std::cout << "   all time: " << diff.count() << "\n";*/

    return 0;
}
