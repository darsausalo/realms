#ifndef MOTOR_ARCHIVE_H
#define MOTOR_ARCHIVE_H

#include <array>
#include <iostream>
#include <type_traits>
#include <utility>

namespace motor {

/* errors */

class serialize_error : public std::runtime_error {
public:
    serialize_error(const std::string& what_arg)
        : std::runtime_error(what_arg) {}
    serialize_error(const char* what_arg) : std::runtime_error(what_arg) {}
};

/* type traits */
template<typename T>
struct is_array {
    enum { value = false };
};

template<typename T, std::size_t N>
struct is_array<std::array<T, N>> {
    enum { value = true };
};

/* prologue and epilogue */

template<typename Archive, typename T>
inline void prologue(Archive&, const T&) {
}

template<typename Archive, typename T>
inline void epilogue(Archive&, const T&) {
}

/* base input archive */

template<typename Archive>
class input_archive {
    using archive_type = Archive;

public:
    input_archive& operator=(const input_archive&) = delete;

    ~input_archive() noexcept = default;

    template<typename... Types>
    const archive_type& operator()(Types&&... args) {
        process(std::forward<Types>(args)...);
        return *self;
    }

    template<typename T>
    void member(std::string_view name, T& value) {
        const_cast<archive_type*>(self)->find_key(name);
        process(value);
    }

protected:
    const archive_type* self;

    input_archive(const archive_type* derived) noexcept : self(derived) {}

private:
    template<typename T>
    void process(T&& head) {
        prologue(const_cast<archive_type&>(*self), head);
        processImpl(head);
        epilogue(const_cast<archive_type&>(*self), head);
    }

    template<typename T, typename... Other>
    void process(T&& head, Other&&... tail) {
        process(std::forward<T>(head));
        process(std::forward<Other>(tail)...);
    }

    template<typename T>
    void processImpl(T& value) {
        serialize(const_cast<archive_type&>(*self), value);
    }
};

/* base output archive */

template<typename Archive>
class output_archive {
    using archive_type = Archive;

public:
    output_archive& operator=(const output_archive&) = delete;

    ~output_archive() noexcept = default;

    template<typename... Types>
    const archive_type& operator()(Types&&... args) {
        process(std::forward<Types>(args)...);
        return *self;
    }

    template<typename T>
    void member(std::string_view name, const T& value) {
        const_cast<archive_type*>(self)->find_key(name);
        process(value);
    }

protected:
    const archive_type* self;

    output_archive(const archive_type* derived) noexcept : self(derived) {}

private:
    template<typename T>
    void process(T&& head) {
        prologue(const_cast<archive_type&>(*self), head);
        processImpl(head);
        epilogue(const_cast<archive_type&>(*self), head);
    }

    template<typename T, typename... Other>
    void process(T&& head, Other&&... tail) {
        process(std::forward<T>(head));
        process(std::forward<Other>(tail)...);
    }

    template<typename T>
    void processImpl(const T& value) {
        serialize(const_cast<archive_type&>(*self), const_cast<T&>(value));
    }
};


} // namespace motor

#endif // MOTOR_ARCHIVE_H
