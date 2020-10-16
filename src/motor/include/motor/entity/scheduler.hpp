#ifndef MOTOR_SCHEDULER_HPP
#define MOTOR_SCHEDULER_HPP

#include <entt/entity/organizer.hpp>
#include <unordered_map>
#include <vector>

namespace motor {

class scheduler {
public:
    using function_type = entt::organizer::function_type;
    using vertex = entt::organizer::vertex;

    scheduler() = default;
    scheduler(scheduler&&) = default;

    scheduler& operator=(scheduler&&) = default;

    void add_stage(entt::id_type stage) {
        assert(stages.find(stage) == stages.end());
        stages.insert({stage, {}});
        stage_order.push_back(stage);
    }

    void add_stage_after(entt::id_type target, entt::id_type stage) {
        assert(stages.find(stage) == stages.end());
        auto pos = std::find_if(stage_order.cbegin(), stage_order.cend(),
                                [target](auto&& it) { return it == target; });
        assert(pos != stage_order.cend());
        stage_order.insert(pos + 1, stage);
    }

    void add_stage_before(entt::id_type target, entt::id_type stage) {
        assert(stages.find(stage) == stages.end());
        auto pos = std::find_if(stage_order.cbegin(), stage_order.cend(),
                                [target](auto&& it) { return it == target; });
        assert(pos != stage_order.cend());
        stage_order.insert(pos, stage);
    }

    template<auto Candidate, typename... Req>
    void add_system_to_stage(entt::id_type stage, const char* name = nullptr) {
        assert(stages.find(stage) != stages.end());
        stages[stage].emplace<Candidate, Req...>(name);
    }

    template<auto Candidate, typename... Req, typename Type>
    void add_system_to_stage(entt::id_type stage,
                             Type& value_or_instance,
                             const char* name = nullptr) {
        assert(stages.find(stage) != stages.end());
        stages[stage].emplace<Candidate, Req..., Type>(value_or_instance, name);
    }

    template<typename... Req>
    void add_system_to_stage(entt::id_type stage,
                             function_type* func,
                             const void* payload = nullptr,
                             const char* name = nullptr) {
        assert(stages.find(stage) != stages.end());
        stages[stage].emplace<Req...>(func, payload, name);
    }

    template<auto Candidate, typename... Req>
    void add_startup_system(const char* name = nullptr) {
        startup_systems.emplace<Candidate, Req...>(name);
    }

    template<auto Candidate, typename... Req, typename Type>
    void add_startup_system(Type& value_or_instance,
                            const char* name = nullptr) {
        startup_systems.emplace<Candidate, Req..., Type>(
            value_or_instance, name);
    }

    template<typename... Req>
    void add_startup_system(function_type* func,
                            const void* payload = nullptr,
                            const char* name = nullptr) {
        startup_systems.emplace<Req...>(func, payload, name);
    }

    template<typename Func>
    void visit(Func func) const {
        for (auto&& stage : stage_order) {
            func(stage);
        }
    }

    std::vector<vertex> startup_graph() { return startup_systems.graph(); }

    std::vector<vertex> graph(entt::id_type stage) {
        if (auto it = stages.find(stage); it != stages.end()) {
            return it->second.graph();
        }
        return {};
    }

private:
    entt::organizer startup_systems{};
    std::unordered_map<entt::id_type, entt::organizer> stages{};
    std::vector<entt::id_type> stage_order{};
};

} // namespace motor

#endif // MOTOR_SCHEDULER_HPP
