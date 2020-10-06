#ifndef MOTOR_ALGORITHM_HPP
#define MOTOR_ALGORITHM_HPP

#include <string_view>
#include <vector>

namespace motor {

std::vector<std::string_view> split(std::string_view input,
                                    std::string_view delims) {
    std::vector<std::string_view> result;
    size_t first = 0;

    while (first < input.size()) {
        const auto second = input.find_first_of(delims, first);
        if (first != second) {
            result.emplace_back(input.substr(first, second - first));
        }
        if (second == std::string_view::npos) {
            break;
        }
        first = second + 1;
    }

    return result;
}

} // namespace motor

#endif // MOTOR_ALGORITHM_HPP
