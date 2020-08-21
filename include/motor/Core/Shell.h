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

#include <entt/locator/locator.hpp>
#include <string_view>

namespace motor {

class Shell {
public:
  virtual ~Shell() = default;

  virtual std::ifstream openReadText(std::string_view filename) = 0;
  virtual std::ifstream openReadBinary(std::string_view filename) = 0;

  virtual std::ofstream openWriteText(std::string_view filename) = 0;
  virtual std::ofstream openWriteBinary(std::string_view filename) = 0;
};

using shell = entt::service_locator<Shell>;

} // namespace motor

#endif //MOTOR_SHELL_H
