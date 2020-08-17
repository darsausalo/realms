/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Shell.h
    Game host shell - main context to operate between host and user-space.

===============================================================================
*/

#ifndef MOTOR_SHELL_H
#define MOTOR_SHELL_H

#include <entt/core/type_info.hpp>
#include <memory>
#include <vector>

namespace motor {

class Shell {
public:
  template<typename Type>
  [[nodiscard]] const Type* get() const {
    auto it =
        std::find_if(services.cbegin(), services.cend(), [](auto&& svc) {
          return svc.type_id == entt::type_info<Type>::id();
        });
    return it == services.cend() ? nullptr
                                 : static_cast<const Type*>(it->value.get());
  }

  template<typename Type>
  [[nodiscard]] Type* get() {
    return const_cast<Type*>(std::as_const(*this).template get<Type>());
  }

#ifdef MOTOR_HOST

  template<typename Type, typename... Args>
  Type* set(Args&&... args) {
    services.push_back(
        serviceData{entt::type_info<Type>::id(),
                    {new Type{std::forward<Args>(args)...}, [](void* instance) {
                       delete static_cast<Type*>(instance);
                     }}});
    return static_cast<Type*>(services.back().value.get());
  }

#endif

private:
  struct serviceData {
    entt::id_type type_id;
    std::unique_ptr<void, void (*)(void*)> value;
  };

  std::vector<serviceData> services{};
};

extern Shell* shell;

} // namespace motor

#endif //MOTOR_SHELL_H
