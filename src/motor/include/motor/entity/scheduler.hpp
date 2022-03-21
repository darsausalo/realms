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
    void add_system_to_stage(entt::id_type stage,
                             const entt::id_type label = entt::id_type{},
                             const char* name = nullptr) {
        assert(stages.find(stage) != stages.end());
        stages[stage].emplace<Candidate, Req...>(name);
        auto system_id =
            entt::type_id<
                std::integral_constant<decltype(Candidate), Candidate>>()
                .index();
        labels[stage].emplace(system_id, label);
    }

    template<auto Candidate, typename... Req, typename Type>
    void add_system_to_stage(entt::id_type stage,
                             Type& value_or_instance,
                             const entt::id_type label = entt::id_type{},
                             const char* name = nullptr) {
        assert(stages.find(stage) != stages.end());
        stages[stage].emplace<Candidate, Req..., Type>(value_or_instance, name);
        auto system_id =
            entt::type_id<
                std::integral_constant<decltype(Candidate), Candidate>>()
                .index();
        labels[stage].emplace(system_id, label);
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

    template<typename Func>
    void visit(Func func) const {
        for (auto&& stage : stage_order) {
            func(stage);
        }
    }

    std::vector<vertex> startup_graph() { return startup_systems.graph(); }

    std::vector<std::pair<vertex, entt::id_type>> graph(entt::id_type stage) {
        if (auto it = stages.find(stage); it != stages.end()) {
            auto adjacency_list = it->second.graph();
            std::vector<std::pair<vertex, entt::id_type>> systems_list{};
            systems_list.reserve(adjacency_list.size());

            for (auto& v : adjacency_list) {
                assert(labels.contains(stage) &&
                       labels[stage].contains(v.info().index()));
                systems_list.emplace_back(v, labels[stage][v.info().index()]);
            }

            return systems_list;
        }
        return {};
    }

private:
    entt::organizer startup_systems{};
    std::unordered_map<entt::id_type, entt::organizer> stages{};
    std::unordered_map<entt::id_type,
                       std::unordered_map<entt::id_type, entt::id_type>>
        labels{};
    std::vector<entt::id_type> stage_order{};
};

} // namespace motor

#endif // MOTOR_SCHEDULER_HPP
