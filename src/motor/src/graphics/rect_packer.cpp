// Based on algorithm from https://github.com/mapbox/potpack
// ISC License
//
// Copyright (c) 2018, Mapbox
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "motor/graphics/rect_packer.hpp"
#include "motor/core/utility.hpp"
#include <algorithm>
#include <cmath>

namespace motor {

void rect_packer::emplace(entt::id_type id,
                          std::size_t width,
                          std::size_t height) noexcept {
    assert(std::find_if(rects.cbegin(), rects.cend(), [id](auto&& r) {
               return std::get<0>(r) == id;
           }) == rects.cend());
    rects.push_back(std::make_tuple(id, rect{0u, 0u, width, height}));
}

std::vector<rect_packer::rect_type> rect_packer::pack() noexcept {
    std::size_t area{};

    for (auto&& [_, rect] : rects) {
        rect.x = rect.y = 0;
        area += rect.w * rect.h;
    }

    std::sort(rects.begin(), rects.end(), [](auto& lhs, auto& rhs) {
        return std::get<1>(lhs).h > std::get<1>(rhs).h;
    });

    std::vector<rect_type> packed;
    std::vector<rect> spaces{{0u, 0u, max_width, max_height}};

    int iter = 0;
    for (auto&& [id, rect] : rects) {
        for (int i = spaces.size() - 1; i >= 0; i--) {
            auto& space = spaces[i];
            auto w = rect.w + border;
            auto h = rect.h + border;

            if (w > space.w || h > space.h) {
                continue;
            }

            // found the space; add the rect to its top-left corner
            // |-------|-------|
            // |  rect |       |
            // |_______|       |
            // |         space |
            // |_______________|
            rect.x = space.x;
            rect.y = space.y;

            packed.push_back(std::make_tuple(id, rect));

            if (w == space.w && h == space.h) {
                auto last = spaces.back();
                spaces.pop_back();
                if (i < spaces.size()) {
                    spaces[i] = last;
                }
            } else if (h == space.h) {
                // space matches the rect height; update it accordingly
                // |-------|---------------|
                // |  rect | updated space |
                // |_______|_______________|
                space.x += w;
                space.w -= w;
            } else if (w == space.w) {
                // space matches the rect width; update it accordingly
                // |---------------|
                // |     rect      |
                // |_______________|
                // | updated space |
                // |_______________|
                space.y += h;
                space.h -= h;

            } else {
                // otherwise the rect splits the space into two spaces
                // |-------|-----------|
                // |  rect | new space |
                // |_______|___________|
                // | updated space     |
                // |___________________|
                auto x = space.x;
                auto y = space.y;
                auto width = space.w;
                space.y += h;
                space.h -= h;
                spaces.push_back({
                    x + w,     //
                    y,         //
                    width - w, //
                    h          //
                });
            }
            break;
        }
    }

    for (auto&& [id, _] : packed) {
        rects.erase(std::find_if(rects.cbegin(), rects.cend(), [&id](auto&& r) {
            return std::get<0>(r) == id;
        }));
    }

    return packed;
}

} // namespace motor

//==============================================================================
// TEST

#include <doctest/doctest.h>

TEST_CASE("rect_packer: fit one") {
    using namespace motor;

    rect_packer packer{16, 32};

    packer.emplace(0, 16, 32);

    auto result = packer.pack();

    CHECK(packer.empty());
    CHECK(result.size() == 1);
    CHECK(std::get<1>(result[0]).x == 0);
    CHECK(std::get<1>(result[0]).y == 0);
}

TEST_CASE("rect_packer: fit two") {
    using namespace motor;

    rect_packer packer{16, 32};

    packer.emplace(0, 16, 16);
    packer.emplace(1, 16, 16);

    auto result = packer.pack();

    CHECK(packer.empty());
    CHECK(result.size() == 2);
    CHECK(std::get<1>(result[0]).x == 0);
    CHECK(std::get<1>(result[0]).y == 0);
    CHECK(std::get<1>(result[0]).w == 16);
    CHECK(std::get<1>(result[0]).h == 16);
    CHECK(std::get<1>(result[1]).x == 0);
    CHECK(std::get<1>(result[1]).y == 16);
    CHECK(std::get<1>(result[1]).w == 16);
    CHECK(std::get<1>(result[1]).h == 16);
}

TEST_CASE("rect_packer: fit three in two") {
    using namespace motor;

    rect_packer packer{16, 32};

    packer.emplace(0, 16, 16);
    packer.emplace(1, 16, 16);
    packer.emplace(2, 16, 20);

    auto result = packer.pack();

    CHECK(!packer.empty());
    CHECK(result.size() == 1);
    CHECK(std::get<1>(result[0]).x == 0);
    CHECK(std::get<1>(result[0]).y == 0);
    CHECK(std::get<1>(result[0]).w == 16);
    CHECK(std::get<1>(result[0]).h == 20);

    result = packer.pack();

    CHECK(packer.empty());
    CHECK(result.size() == 2);
    CHECK(std::get<1>(result[0]).x == 0);
    CHECK(std::get<1>(result[0]).y == 0);
    CHECK(std::get<1>(result[0]).w == 16);
    CHECK(std::get<1>(result[0]).h == 16);
    CHECK(std::get<1>(result[1]).x == 0);
    CHECK(std::get<1>(result[1]).y == 16);
    CHECK(std::get<1>(result[1]).w == 16);
    CHECK(std::get<1>(result[1]).h == 16);
}

TEST_CASE("rect_packer: fit four in two") {
    using namespace motor;

    rect_packer packer{16, 32};

    packer.emplace(0, 16, 16);
    packer.emplace(1, 16, 16);
    packer.emplace(2, 16, 20);
    packer.emplace(3, 8, 6);

    auto result = packer.pack();

    CHECK(!packer.empty());
    CHECK(result.size() == 2);
    CHECK(std::get<1>(result[0]).x == 0);
    CHECK(std::get<1>(result[0]).y == 0);
    CHECK(std::get<1>(result[0]).w == 16);
    CHECK(std::get<1>(result[0]).h == 20);
    CHECK(std::get<1>(result[1]).x == 0);
    CHECK(std::get<1>(result[1]).y == 20);
    CHECK(std::get<1>(result[1]).w == 8);
    CHECK(std::get<1>(result[1]).h == 6);

    result = packer.pack();

    CHECK(packer.empty());
    CHECK(result.size() == 2);
    CHECK(std::get<1>(result[0]).x == 0);
    CHECK(std::get<1>(result[0]).y == 0);
    CHECK(std::get<1>(result[0]).w == 16);
    CHECK(std::get<1>(result[0]).h == 16);
    CHECK(std::get<1>(result[1]).x == 0);
    CHECK(std::get<1>(result[1]).y == 16);
    CHECK(std::get<1>(result[1]).w == 16);
    CHECK(std::get<1>(result[1]).h == 16);
}

TEST_CASE("rect_packer: fit six in two") {
    using namespace motor;

    rect_packer packer{16, 32};

    packer.emplace(0, 16, 16);
    packer.emplace(1, 16, 16);
    packer.emplace(2, 16, 20);
    packer.emplace(3, 8, 8);
    packer.emplace(4, 4, 4);
    packer.emplace(5, 4, 4);

    auto result = packer.pack();

    CHECK(!packer.empty());
    CHECK(result.size() == 4);
    CHECK(std::get<1>(result[0]).x == 0);
    CHECK(std::get<1>(result[0]).y == 0);
    CHECK(std::get<1>(result[0]).w == 16);
    CHECK(std::get<1>(result[0]).h == 20);
    CHECK(std::get<1>(result[1]).x == 0);
    CHECK(std::get<1>(result[1]).y == 20);
    CHECK(std::get<1>(result[1]).w == 8);
    CHECK(std::get<1>(result[1]).h == 8);
    CHECK(std::get<1>(result[2]).x == 8);
    CHECK(std::get<1>(result[2]).y == 20);
    CHECK(std::get<1>(result[2]).w == 4);
    CHECK(std::get<1>(result[2]).h == 4);
    CHECK(std::get<1>(result[3]).x == 12);
    CHECK(std::get<1>(result[3]).y == 20);
    CHECK(std::get<1>(result[3]).w == 4);
    CHECK(std::get<1>(result[3]).h == 4);

    result = packer.pack();

    CHECK(packer.empty());
    CHECK(result.size() == 2);
    CHECK(std::get<1>(result[0]).x == 0);
    CHECK(std::get<1>(result[0]).y == 0);
    CHECK(std::get<1>(result[0]).w == 16);
    CHECK(std::get<1>(result[0]).h == 16);
    CHECK(std::get<1>(result[1]).x == 0);
    CHECK(std::get<1>(result[1]).y == 16);
    CHECK(std::get<1>(result[1]).w == 16);
    CHECK(std::get<1>(result[1]).h == 16);
}

TEST_CASE("rect_packer: fit two with border") {
    using namespace motor;

    rect_packer packer{16, 32, 1};

    packer.emplace(0, 15, 15);
    packer.emplace(1, 15, 15);

    auto result = packer.pack();

    CHECK(packer.empty());
    CHECK(result.size() == 2);
    CHECK(std::get<1>(result[0]).x == 0);
    CHECK(std::get<1>(result[0]).y == 0);
    CHECK(std::get<1>(result[0]).w == 15);
    CHECK(std::get<1>(result[0]).h == 15);
    CHECK(std::get<1>(result[1]).x == 0);
    CHECK(std::get<1>(result[1]).y == 16);
    CHECK(std::get<1>(result[1]).w == 15);
    CHECK(std::get<1>(result[1]).h == 15);
}
