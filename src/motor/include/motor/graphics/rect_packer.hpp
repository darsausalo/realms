#ifndef MOTOR_RECT_PACK_HPP
#define MOTOR_RECT_PACK_HPP

#include "motor/core/rect.hpp"
#include <entt/core/fwd.hpp>
#include <tuple>
#include <utility>
#include <vector>

namespace motor {

class rect_packer {
public:
    using rect_type = std::tuple<entt::id_type, rect>;

    rect_packer(std::size_t max_width,
                std::size_t max_height,
                std::size_t border = 0)
        : max_width{max_width}
        , max_height{max_height}
        , border{border} {}
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
    std::size_t border;
    std::vector<rect_type> rects{};
};

} // namespace motor

#endif // MOTOR_RECT_PACK_HPP
