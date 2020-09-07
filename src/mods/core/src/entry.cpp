#include <fmt/core.h>
#include <iostream>
#include <memory>
#include <motor/core/attribute.h>
#include <motor/core/system_module.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

namespace core {

MOTOR_EXPORT void plugin_entry(motor::system_module_context* ctx) {
    spdlog::set_default_logger(ctx->get_logger());
    spdlog::debug("plugin_entry: core[1]");
}

} // namespace core
