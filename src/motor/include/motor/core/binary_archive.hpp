#ifndef MOTOR_BINARY_ARCHIVE_HPP
#define MOTOR_BINARY_ARCHIVE_HPP

#include "motor/core/archive.hpp"
#include <entt/entity/fwd.hpp>
#include <fmt/core.h>
#include <iostream>

namespace motor {

// binary output archive

class binary_output_archive : public output_archive<binary_output_archive> {
public:
    binary_output_archive(std::ostream& s) noexcept
        : output_archive{this}, stream{s} {}
    ~binary_output_archive() noexcept = default;

    void find_key(std::string_view) {}

    void save_data(const void* data, std::streamsize size) {
        const auto write_size =
            stream.rdbuf()->sputn(reinterpret_cast<const char*>(data), size);
        if (write_size != size) {
            throw serialize_error(fmt::format(
                "failed to write {} bytes, wrote {}", size, write_size));
        }
    }

private:
    std::ostream& stream;
};

// binary input archive

class binary_input_archive : public input_archive<binary_input_archive> {
public:
    binary_input_archive(std::istream& s) noexcept
        : input_archive{this}, stream{s} {}
    ~binary_input_archive() noexcept = default;

    void find_key(std::string_view) {}

    void load_data(void* data, std::streamsize size) {
        const auto read_size =
            stream.rdbuf()->sgetn(reinterpret_cast<char*>(data), size);
        if (read_size != size) {
            throw serialize_error(fmt::format(
                "failed to read {} bytes, read {}", size, read_size));
        }
    }

private:
    std::istream& stream;
};

// serialization specialization

template<typename T>
auto serialize(binary_output_archive& ar, T& value)
    -> std::enable_if_t<is_array_v<T>, void> {
    ar.save_data(std::addressof(value), sizeof(value));
}

template<typename T>
auto serialize(binary_output_archive& ar, T& value)
    -> std::enable_if_t<std::is_arithmetic_v<T>, void> {
    ar.save_data(std::addressof(value), sizeof(value));
}

template<typename T>
auto serialize(binary_output_archive& ar, T& value)
    -> std::enable_if_t<std::is_enum_v<T>, void> {
    using base_type = std::underlying_type_t<T>;
    ar.save_data(std::addressof(value), sizeof(base_type));
}

template<typename T>
auto serialize(binary_input_archive& ar, T& value) ->
    typename std::enable_if_t<is_array_v<T>, void> {
    ar.load_data(std::addressof(value), sizeof(value));
}

template<typename T>
auto serialize(binary_input_archive& ar, T& value)
    -> std::enable_if_t<std::is_arithmetic<T>::value, void> {
    ar.load_data(std::addressof(value), sizeof(value));
}

template<typename T>
auto serialize(binary_input_archive& ar, T& value)
    -> std::enable_if_t<std::is_enum_v<T>, void> {
    using base_type = std::underlying_type_t<T>;
    ar.load_data(std::addressof(value), sizeof(base_type));
}

} // namespace motor

#endif // MOTOR_BINARY_ARCHIVE_HPP
