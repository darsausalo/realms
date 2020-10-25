#ifndef MOTOR_MAP_REGION_HPP
#define MOTOR_MAP_REGION_HPP

#include <doctest/doctest.h>
#include <glm/ext/vector_uint2.hpp>

namespace motor {

class map_region {
    class linear_iterator final {
        friend class map_region;

        linear_iterator(const map_region& region, glm::uvec2 track) noexcept
            : region{region}
            , track{track}
            , is_end{track.y >= region.max.y} {}

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = glm::uvec2;
        using pointer = value_type*;
        using reference = value_type;

        constexpr value_type operator*() const noexcept { return track; }

        linear_iterator& operator++() noexcept {
            track.x++;
            if (track.x >= region.max.x) {
                track.y++;
                track.x = region.min.x;
            }
            is_end = track.y >= region.max.y;
            return *this;
        }

        linear_iterator operator++(int) noexcept {
            auto ret = *this;
            ++*this;
            return ret;
        }

        bool operator!=(const linear_iterator& other) const noexcept {
            if (is_end && other.is_end) {
                return false;
            }

            return track != other.track;
        }

        bool operator==(const linear_iterator& other) const noexcept {
            return !(*this != other);
        }

    private:
        const map_region& region;
        glm::uvec2 track;
        bool is_end{};
    };

public:
    using iterator = linear_iterator;

    map_region(glm::uvec2 min, glm::uvec2 max)
        : min{min}
        , max{max} {}

    [[no_discard]] iterator begin() const noexcept {
        return linear_iterator{*this, min};
    }
    [[no_discard]] iterator end() const noexcept {
        return linear_iterator{*this, max};
    }

    [[no_discard]] iterator cbegin() const noexcept {
        return linear_iterator{*this, min};
    }
    [[no_discard]] iterator cend() const noexcept {
        return linear_iterator{*this, max};
    }

private:
    glm::uvec2 min;
    glm::uvec2 max;
};

} // namespace motor

#endif // MOTOR_MAP_REGION_HPP
