#ifndef MOTOR_DYNAMIC_LIBRARY_H
#define MOTOR_DYNAMIC_LIBRARY_H

#include <memory>
#include <string>
#include <string_view>

namespace motor {

class dynamic_library {
public:
    dynamic_library(std::string_view name) noexcept;
    ~dynamic_library() noexcept;

    std::string get_name() const { return name; }

    bool exists() const;

    bool is_loaded() const;
    bool is_changed() const;

    bool load();
    void unload();

    void* get_symbol(std::string_view name) const;

private:
    struct impl;

    std::string name{};
    std::unique_ptr<impl> p{};
};

} // namespace motor

#endif // MOTOR_DYNAMIC_LIBRARY_H
