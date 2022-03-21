#ifndef MOTOR_EXECUTOR_HPP
#define MOTOR_EXECUTOR_HPP

#include "motor/entity/scheduler.hpp"

namespace motor {

class executor {
    using system = std::pair<entt::organizer::vertex, entt::id_type>;

public:
    executor(scheduler& scheduler) {
        scheduler.visit([this, &scheduler](auto stage_id) {
            stages.push_back(std::move(scheduler.graph(stage_id)));
        });
    }

    void run(entt::registry& registry, entt::id_type label = entt::id_type{}) {
        for (auto&& stage : stages) {
            for (auto&& [v, l] : stage) {
                if (l == entt::id_type{} || l == label) {
                    v.callback()(v.data(), registry);
                }
            }
        }
    }

private:
    std::vector<std::vector<system>> stages;
};

} // namespace motor

#endif // MOTOR_EXECUTOR_HPP
