#ifndef MOTOR_ARCHIVE_JSON_H
#define MOTOR_ARCHIVE_JSON_H

#include "archive.h"
#include <nlohmann/json.hpp>

namespace motor {

class json_input_archive : public input_archive<json_input_archive> {
public:
    json_input_archive(const nlohmann::json& j) noexcept
        : input_archive{this}, current_member{} {
        node_stack.push_back(&j);
    }

    void start_node() {
        // TODO: check?
        node_stack.push_back(current_member);
    }

    void end_node() {
        if (node_stack.size() < 1) {
            // TODO: refine
            throw std::exception("empty stack");
        }
        node_stack.pop_back();
    }

    void find_key(std::string_view key) {
        const auto* current_value = node_stack.back();
        current_member = (current_value && current_value->contains(key))
                                 ? &current_value->at(std::data(key))
                                 : nullptr;
    }

    template<typename T, std::size_t N>
    void load_array(std::array<T, N>& arr) {
        // TODO: exception?
        if (current_member) {
            std::size_t i;
            for (i = 0u; i < N && i < current_member->size(); i++) {
                arr[i] = current_member->at(i);
            }
            for (; i < N; i++) {
                arr[i] = T{};
            }
        } else {
            arr.fill(T{});
        }
    }

    template<typename T>
    void load_value(T& value) {
        if (current_member && !current_member->is_null()) {
            value = current_member->get<T>();
        } else {
            value = T{};
        }
    }

private:
    std::vector<const nlohmann::json*> node_stack{};
    const nlohmann::json* current_member{};
};

template<typename T,
         typename = std::enable_if<!std::is_arithmetic<T>::value, void>::type>
inline void prologue(json_input_archive& ar, const T&) {
    ar.start_node();
}

template<typename T,
         typename = std::enable_if<!std::is_arithmetic<T>::value, void>::type>
inline void epilogue(json_input_archive& ar, const T&) {
    ar.end_node();
}

/* serialization specialization */

template<typename T, typename = typename std::enable_if<
                             is_array<T>::value, typename T::value_type>::type>
void serialize(json_input_archive& ar, T& value) {
    ar.load_array(value);
}

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, void>::type
serialize(json_input_archive& ar, T& value) {
    ar.load_value(value);
}

} // namespace motor

#endif // MOTOR_ARCHIVE_JSON_H
