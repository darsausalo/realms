#ifndef MOTOR_ARCHIVE_HPP
#define MOTOR_ARCHIVE_HPP

#include "fwd.hpp"
#include "motor/core/type_traits.hpp"
#include <nameof.hpp>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace motor {

// errors

class serialize_error : public std::runtime_error {
public:
    serialize_error(const std::string& what_arg)
        : std::runtime_error(what_arg) {}
    serialize_error(const char* what_arg)
        : std::runtime_error(what_arg) {}
};

// prologue and epilogue

template<typename Archive, typename T>
inline void prologue(Archive&, const T&) {}

template<typename Archive, typename T>
inline void epilogue(Archive&, const T&) {}

// base output archive

template<typename Archive>
class output_archive {
    using archive_type = Archive;

public:
    output_archive& operator=(const output_archive&) = delete;

    ~output_archive() noexcept = default;

    template<typename... Types>
    const archive_type& operator()(Types&&... args) {
        (process(std::forward<Types>(args)), ...);
        return *self;
    }

    template<typename T>
    void member(std::string_view name, const T& value) {
        const_cast<archive_type*>(self)->find_key(name);
        process(value);
    }

protected:
    const archive_type* self;

    output_archive(const archive_type* derived) noexcept
        : self{derived} {}

private:
    template<typename T>
    void process(const T& value) {
        prologue(const_cast<archive_type&>(*self), value);
        serialize(const_cast<archive_type&>(*self), const_cast<T&>(value));
        epilogue(const_cast<archive_type&>(*self), value);
    }
};

// base input archive

template<typename Archive>
class input_archive {
    using archive_type = Archive;

public:
    input_archive& operator=(const input_archive&) = delete;

    ~input_archive() noexcept = default;

    template<typename... Types>
    const archive_type& operator()(Types&&... args) {
        (process(std::forward<Types>(args)), ...);
        return *self;
    }

    template<typename T>
    void member(std::string_view name, T& value) {
        const_cast<archive_type*>(self)->find_key(name);
        process(value);
    }

protected:
    input_archive(const archive_type* derived) noexcept
        : self{derived} {}

    template<typename T>
    void process(T& value) {
        prologue(const_cast<archive_type&>(*self), value);
        serialize(const_cast<archive_type&>(*self), value);
        epilogue(const_cast<archive_type&>(*self), value);
    }

private:
    const archive_type* self;
};

} // namespace motor

#endif // MOTOR_ARCHIVE_HPP
