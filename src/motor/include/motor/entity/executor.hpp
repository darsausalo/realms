#ifndef MOTOR_EXECUTOR_HPP
#define MOTOR_EXECUTOR_HPP

#include "motor/entity/scheduler.hpp"

namespace motor {

class executor {
    using system = entt::organizer::vertex;

public:
    executor(scheduler& scheduler) {
        scheduler.visit([this, &scheduler](auto stage_id) {
            stages.push_back(std::move(scheduler.graph(stage_id)));
        });
    }

    void run(entt::registry& registry) {
        for (auto&& stage : stages) {
            for (auto&& system : stage) {
                system.callback()(system.data(), registry);
            }
        }
    }

private:
    std::vector<std::vector<system>> stages;
};

} // namespace motor

#endif // MOTOR_EXECUTOR_HPP
