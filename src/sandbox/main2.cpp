#include "archive.h"
#include "binary.h"
#include "json.h"
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

static const char* json_str = R"({
    "entity1": "todo1",
    "entity2": "todo2"
})";

static const char* json_str_2 = R"({
    "arr1": [null, 2, 4, 5, 45, 32, 112],
    "y": 4,
    "x": 12.1,
    "inner": {
        "foo": 132,
        "arr2": [12, 4, 32, 11]
    }
})";

struct inner {
    std::int32_t foo;
    std::int64_t boo;
    std::array<float, 5> arr2;
};

template<typename Archive>
void serialize(Archive& ar, inner& inr) {
    ar.member("foo", inr.foo);
    ar.member("boo", inr.boo);
    ar.member("arr2", inr.arr2);
}

struct position {
    float x;
    float y;
    inner inr_skip;
    inner inr;
    std::array<int, 5> arr1;
};

template<typename Archive>
void serialize(Archive& ar, position& p) {
    ar.member("x", p.x);
    ar.member("y", p.y);
    ar.member("inner_skip", p.inr_skip);
    ar.member("inner", p.inr);
    ar.member("arr1", p.arr1);
}

struct timer {
    int duration;
    int elapsed{0};
};


int main(int argc, char const* argv[]) {
    std::cout << "===== test1 ====\n";
    position p{1.0f, 2.0f, 120};
    p.inr = {32, 41, {32.0f, 5.0f, 34.0f}};
    p.arr1 = {1, 3, 2, 18, 5};
    {
        std::ofstream outbin("tst1.bin", std::ios_base::binary);
        motor::binary_output_archive ar{outbin};
        ar(p);
    }
    {
        position p2{3000.1234f, 311.01f};
        std::ifstream inbin("tst1.bin", std::ios_base::binary);
        motor::binary_input_archive ar{inbin};
        ar(p2);
        assert(p.x == p2.x);
        assert(p.y == p2.y);
        assert(p.inr.foo == p2.inr.foo);
        assert(p.inr.boo == p2.inr.boo);
        assert(p.arr1[0] == p2.arr1[0]);
        assert(p.arr1[1] == p2.arr1[1]);
        assert(p.arr1[2] == p2.arr1[2]);
    }

    std::cout << "===== test2 ====\n";
    nlohmann::json j = nlohmann::json::parse(json_str_2);

    try {
        position p{};
        p.inr.boo = 20;
        motor::json_input_archive ar(j);
        serialize(ar, p);
        std::cout << "p = " << p.x << "," << p.y << std::endl
                  << "p.inr = " << p.inr.foo << "," << p.inr.boo << std::endl
                  << "p.arr1 = ";
        for (auto n : p.arr1) {
            std::cout << n << ",";
        }
        std::cout << std::endl;
    } catch (std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}
