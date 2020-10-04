#include "motor/entity/system_dispatcher.hpp"
#include <algorithm>
#include <functional>

namespace motor {

system_dispatcher::~system_dispatcher() {
    while (!systems.empty()) {
        systems.pop_back();
    }
}

void system_dispatcher::update() {
    for (auto&& order : exec_order) {
        systems[order]();
    }
}

void system_dispatcher::sort() {
    exec_order.clear();
    for (std::size_t i = 0; i < systems.size(); ++i) {
        if (systems[i].function) {
            exec_order.push_back(i);
        }
    }
    std::stable_sort(std::begin(exec_order), std::end(exec_order),
                     [this](auto&& lhs, auto&& rhs) {
                         return systems[lhs].group < systems[rhs].group;
                     });
}

} // namespace motor

//==============================================================================
// TEST

#include <doctest/doctest.h>

static void func_system() {
}

TEST_CASE("system dispatcher: topology sorting") {
    static std::vector<entt::id_type> ctors{};
    static std::vector<entt::id_type> dtors{};

    struct system_pre_frame {
        system_pre_frame() noexcept {
            ctors.push_back(entt::type_info<system_pre_frame>::id());
        }
        ~system_pre_frame() {
            dtors.push_back(entt::type_info<system_pre_frame>::id());
        }
        void operator()() {}
    };
    struct system_post_frame {
        system_post_frame() noexcept {
            ctors.push_back(entt::type_info<system_post_frame>::id());
        }
        ~system_post_frame() {
            dtors.push_back(entt::type_info<system_post_frame>::id());
        }
        void operator()() {}
    };
    struct system_on_store {
        system_on_store() noexcept {
            ctors.push_back(entt::type_info<system_on_store>::id());
        }
        ~system_on_store() {
            dtors.push_back(entt::type_info<system_on_store>::id());
        }
        void operator()() {}
    };
    struct system_a {
        system_a() noexcept {
            ctors.push_back(entt::type_info<system_a>::id());
        }
        ~system_a() { dtors.push_back(entt::type_info<system_a>::id()); }
        void operator()() {}
    };
    struct system_b {
        system_b() noexcept {
            ctors.push_back(entt::type_info<system_b>::id());
        }
        ~system_b() { dtors.push_back(entt::type_info<system_b>::id()); }
        void operator()() {}
    };
    struct system_c {
        system_c() noexcept {
            ctors.push_back(entt::type_info<system_c>::id());
        }
        ~system_c() { dtors.push_back(entt::type_info<system_c>::id()); }
        void operator()() {}
    };
    struct startup_system {
        startup_system() noexcept {
            ctors.push_back(entt::type_info<startup_system>::id());
        }
        ~startup_system() {
            dtors.push_back(entt::type_info<startup_system>::id());
        }
    };
    entt::id_type lambda_system_id{};
    entt::id_type func_system_id{};

    std::vector<entt::id_type> calls{};
    {
        motor::system_dispatcher dispatcher;

        dispatcher.add<motor::system_group::post_frame, system_post_frame>();
        dispatcher.add<motor::system_group::on_update, system_a>();
        dispatcher.add<motor::system_group::pre_frame, system_pre_frame>();
        dispatcher.add<motor::system_group::on_store, system_on_store>();
        dispatcher.add<motor::system_group::on_update, system_b>();
        dispatcher.add<motor::system_group::on_update, system_c>();
        dispatcher.add<motor::system_group::on_load, startup_system>();
        lambda_system_id =
                dispatcher.add<motor::system_group::on_update>([&calls]() {});
        func_system_id =
                dispatcher.add<motor::system_group::on_update>(func_system);

        dispatcher.visit(
                [&calls](const auto system_id) { calls.push_back(system_id); });
    }

    CHECK(ctors.size() == 7);
    CHECK(ctors[0] == entt::type_info<system_post_frame>::id());
    CHECK(ctors[1] == entt::type_info<system_a>::id());
    CHECK(ctors[2] == entt::type_info<system_pre_frame>::id());
    CHECK(ctors[3] == entt::type_info<system_on_store>::id());
    CHECK(ctors[4] == entt::type_info<system_b>::id());
    CHECK(ctors[5] == entt::type_info<system_c>::id());
    CHECK(ctors[6] == entt::type_info<startup_system>::id());

    CHECK(calls.size() == 8);
    CHECK(calls[0] == entt::type_info<system_pre_frame>::id());
    CHECK(calls[1] == entt::type_info<system_a>::id());
    CHECK(calls[2] == entt::type_info<system_b>::id());
    CHECK(calls[3] == entt::type_info<system_c>::id());
    CHECK(calls[4] == lambda_system_id);
    CHECK(calls[5] == func_system_id);
    CHECK(calls[6] == entt::type_info<system_on_store>::id());
    CHECK(calls[7] == entt::type_info<system_post_frame>::id());

    CHECK(dtors.size() == 7);
    CHECK(dtors[6] == entt::type_info<system_post_frame>::id());
    CHECK(dtors[5] == entt::type_info<system_a>::id());
    CHECK(dtors[4] == entt::type_info<system_pre_frame>::id());
    CHECK(dtors[3] == entt::type_info<system_on_store>::id());
    CHECK(dtors[2] == entt::type_info<system_b>::id());
    CHECK(dtors[1] == entt::type_info<system_c>::id());
    CHECK(dtors[0] == entt::type_info<startup_system>::id());
}
