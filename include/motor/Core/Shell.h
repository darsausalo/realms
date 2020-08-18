/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Shell.h
    Main context to operate between host and user-space
    and game loop runner.

===============================================================================
*/

#ifndef MOTOR_SHELL_H
#define MOTOR_SHELL_H

#include <entt/core/type_info.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

namespace motor {

class Shell {
public:
  template<typename Type>
  [[nodiscard]] const Type* get() const {
    auto it = std::find_if(services.cbegin(), services.cend(), [](auto&& svc) {
      return svc.type_id == entt::type_info<Type>::id();
    });
    return it == services.cend() ? nullptr
                                 : static_cast<const Type*>(it->value.get());
  }

  template<typename Type>
  [[nodiscard]] Type* get() {
    return const_cast<Type*>(std::as_const(*this).template get<Type>());
  }

  std::optional<std::filesystem::path> findPath(std::string_view filename);

  std::optional<std::ifstream> openRead(std::string_view filename);
  std::optional<std::ofstream> openWrite(std::string_view filename);

  std::optional<std::string> readText(std::string_view filename);

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

  static int run(int argc, char* argv[]);

#endif

private:
  struct serviceData {
    entt::id_type type_id;
    std::unique_ptr<void, void (*)(void*)> value;
  };

  std::vector<serviceData> services{};

  std::string baseDir{};
  std::string userDir{};
  std::string pluginFilename{};
  std::vector<std::filesystem::path> searchPaths;

#if MOTOR_HOST

  void init(int argc, char* argv[]);
  int run();

#endif
};

extern Shell* shell;

} // namespace motor

#endif //MOTOR_SHELL_H
