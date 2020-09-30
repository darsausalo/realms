#ifndef MOTOR_LUA_ARCHIVE_H
#define MOTOR_LUA_ARCHIVE_H

#include "motor/core/archive.h"
#include <sol/sol.hpp>

#include <doctest/doctest.h>

namespace motor {

class lua_input_archive : public input_archive<lua_input_archive> {
public:
    explicit lua_input_archive(const sol::object& object) noexcept
        : input_archive{this}, current_member{object} {
        node_stack.push_back(object);
    }

    void start_node() { node_stack.push_back(current_member); }

    void end_node() {
        if (node_stack.size() < 1) {
            throw serialize_error("missing stack size on read from lua");
        }
        node_stack.pop_back();
    }

    void find_key(std::string_view key) {
        auto current_value = node_stack.back();
        if (current_value && current_value.is<sol::table>()) {
            sol::optional<sol::object> maybe_member =
                    current_value.as<sol::table>()[std::data(key)];
            current_member = maybe_member ? maybe_member.value() : sol::lua_nil;
        } else {
            current_member = sol::object{};
        }
    }

    template<typename T, std::size_t N>
    void load_array(std::array<T, N>& arr) {
        if (current_member && current_member.is<sol::table>()) {
            std::size_t i = 0;
            auto tbl = current_member.as<sol::table>();
            for (auto&& it : tbl) {
                if (!it.first.is<int>()) {
                    continue;
                }
                auto index = it.first.as<int>() - 1;
                if (index < 0 || index >= N) {
                    continue;
                }
                auto save_member = current_member;
                current_member = it.second;
                process(arr[index]);
                current_member = save_member;
            }
            for (i = tbl.size(); i < N; i++) {
                arr[i] = T{};
            }
        } else {
            arr.fill(T{});
        }
    }

    template<typename T>
    void load_value(T& value) {
        if (current_member) {
            value = current_member.as<T>();
        } else {
            auto current_value = node_stack.back();
            if (current_value && !current_value.is<sol::table>()) {
                value = current_value.as<T>();
            } else {
                value = T{};
            }
        }
    }

private:
    std::vector<sol::object> node_stack{};
    sol::object current_member{};
};

// prologue/epilogue

template<typename T>
auto prologue(lua_input_archive& ar, const T&)
        -> std::enable_if_t<std::is_class_v<T>, void> {
    ar.start_node();
}

template<typename T>
auto epilogue(lua_input_archive& ar, const T&)
        -> std::enable_if_t<std::is_class_v<T>, void> {
    ar.end_node();
}

// serialization specialization

template<typename T>
auto serialize(lua_input_archive& ar, T& value)
        -> std::enable_if_t<is_array_v<T>, void> {
    ar.load_array(value);
}

template<typename T>
auto serialize(lua_input_archive& ar, T& value)
        -> std::enable_if_t<std::is_arithmetic_v<T>, void> {
    ar.load_value(value);
}

template<typename T>
auto serialize(lua_input_archive& ar, T& value) ->
        typename std::enable_if_t<std::is_enum_v<T>, void> {
    ar.load_value(value);
}

inline void serialize(lua_input_archive& ar, std::string& value) {
    ar.load_value(value);
}

} // namespace motor

#endif // MOTOR_LUA_ARCHIVE_H
