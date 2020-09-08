#include <memory>
#include <motor/core/attribute.h>
#include <motor/core/plugin_context.h>
#include <spdlog/spdlog.h>

namespace game {

MOTOR_EXPORT void plugin_entry(motor::plugin_context* ctx) {
    spdlog::set_default_logger(ctx->get_logger());
    spdlog::debug("plugin_entry: sample2");
}

} // namespace game
