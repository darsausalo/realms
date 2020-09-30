#ifndef MOTOR_JSON_ARCHIVE_H
#define MOTOR_JSON_ARCHIVE_H

#include "motor/core/archive.h"
#include <nlohmann/json.hpp>

namespace motor {

class json_input_archive : public input_archive<json_input_archive> {
public:
    explicit json_input_archive(const nlohmann::json& j) noexcept
        : input_archive{this}, current_member{} {
        node_stack.push_back(&j);
    }

    void start_node() { node_stack.push_back(current_member); }

    void end_node() {
        if (node_stack.size() < 1) {
            throw serialize_error("missing stack size on read from json");
        }
        node_stack.pop_back();
    }

    void find_key(std::string_view key) {
        const auto* current_value = node_stack.back();
        if (current_value && !current_value->is_null()) {
            if (!current_value->is_object()) {
                throw serialize_error("object expected");
            }
            current_member = current_value->contains(key)
                                     ? &current_value->at(std::data(key))
                                     : nullptr;
        } else {
            current_member = nullptr;
        }
    }

    template<typename T, std::size_t N>
    void load_array(std::array<T, N>& arr) {
        if (current_member && !current_member->is_null()) {
            if (!current_member->is_array()) {
                throw serialize_error("array expected");
            }
            std::size_t i;
            for (i = 0u; i < N && i < current_member->size(); i++) {
                auto& jvalue = current_member->at(i);
                if (!jvalue.is_null()) {
                    const auto* save_member = current_member;
                    current_member = &jvalue;
                    process(arr[i]);
                    current_member = save_member;
                } else {
                    arr[i] = T{};
                }
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
            const auto* current_value = node_stack.back();
            if (!current_value->is_object() && !current_value->is_array()) {
                value = current_value->get<T>();
            } else {
                value = T{};
            }
        }
    }

private:
    std::vector<const nlohmann::json*> node_stack{};
    const nlohmann::json* current_member{};
};

// prologue/epilogue

template<typename T>
auto prologue(json_input_archive& ar, const T&)
        -> std::enable_if_t<std::is_class_v<T>, void> {
    ar.start_node();
}

template<typename T>
auto epilogue(json_input_archive& ar, const T&)
        -> std::enable_if_t<std::is_class_v<T>, void> {
    ar.end_node();
}

// serialization specialization

template<typename T>
auto serialize(json_input_archive& ar, T& value)
        -> std::enable_if_t<is_array_v<T>, void> {
    ar.load_array(value);
}

template<typename T>
auto serialize(json_input_archive& ar, T& value)
        -> std::enable_if_t<std::is_arithmetic_v<T>, void> {
    ar.load_value(value);
}

template<typename T>
auto serialize(json_input_archive& ar, T& value) ->
        typename std::enable_if_t<std::is_enum_v<T>, void> {
    ar.load_value(value);
}

} // namespace motor

#endif // MOTOR_JSON_ARCHIVE_H
