#ifndef MOTOR_INTERNAL_EVENTS_HPP
#define MOTOR_INTERNAL_EVENTS_HPP

#include <filesystem>

namespace motor::event {

struct config_changed {};

enum class file_action { added, removed, modified, moved };

struct file_changed {
    file_action action;
    std::filesystem::path path;
    std::filesystem::path fullpath;
};

} // namespace motor::event

#endif // MOTOR_INTERNAL_EVENTS_HPP
