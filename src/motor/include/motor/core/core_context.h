#ifndef MOTOR_CORE_CONTEXT_H
#define MOTOR_CORE_CONTEXT_H

#include "motor/core/storage.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace motor {

class core_context {
public:
    std::vector<std::string> args;
    nlohmann::json jconfig;

    core_context(const std::filesystem::path& base_path,
                 const std::filesystem::path& data_path,
                 const std::filesystem::path& user_path)
        : storage(base_path, data_path, user_path) {}
    core_context(const core_context&) = delete;
    core_context(core_context&&) = default;

    core_context& operator=(const core_context&) = delete;
    core_context& operator=(core_context&&) = default;

    ~core_context() noexcept = default;

    const storage& get_storage() const { return storage; }
    storage& get_storage() { return storage; }

private:
    storage storage;
};

} // namespace motor

#endif // MOTOR_CORE_CONTEXT_H
