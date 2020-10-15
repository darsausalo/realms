#ifndef MOTOR_RECT_PACK_HPP
#define MOTOR_RECT_PACK_HPP

#include <entt/core/fwd.hpp>
#include <vector>

namespace motor {

class rect_packer {
public:
    struct rect_type {
        entt::id_type id;
        std::size_t x;
        std::size_t y;
        std::size_t w;
        std::size_t h;
    };

    rect_packer(std::size_t max_width, std::size_t max_height)
        : max_width{max_width}
        , max_height{max_height} {}
    rect_packer(rect_packer&&) = default;

    rect_packer& operator=(rect_packer&&) = default;

    void emplace(entt::id_type id,
                 std::size_t width,
                 std::size_t height) noexcept;

    [[no_discard]] bool empty() const noexcept { return rects.empty(); }

    std::vector<rect_type> pack() noexcept;

private:
    std::size_t max_width;
    std::size_t max_height;
    std::vector<rect_type> rects{};
};

} // namespace motor

#endif // MOTOR_RECT_PACK_HPP
