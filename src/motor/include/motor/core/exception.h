#ifndef MOTOR_EXCEPTION_H
#define MOTOR_EXCEPTION_H

#include <string>

namespace motor {

class exception : public std::exception {
public:
    exception() noexcept = default;
    exception(const exception&) noexcept = default;
    exception(exception&&) noexcept = default;
    explicit exception(std::string message) : message(message) {}

    ~exception() noexcept = default;

    const char* what() const noexcept override { return std::data(message); }

private:
    std::string message{};
};

} // namespace motor

#endif // MOTOR_EXCEPTION_H
