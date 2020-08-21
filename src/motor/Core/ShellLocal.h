/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    ShellLocal.h
    Main context to operate between host and user-space
    and game loop runner.

===============================================================================
*/

#ifndef MOTOR_SHELLLOCAL_H
#define MOTOR_SHELLLOCAL_H

#include "motor/Core/Shell.h"
#include <filesystem>

namespace motor {

class ShellLocal final : public Shell {
public:
  ShellLocal() = default;
  ~ShellLocal() final = default;

  std::ifstream openReadText(std::string_view filename) final;
  std::ifstream openReadBinary(std::string_view filename) final;

  std::ofstream openWriteText(std::string_view filename) final;
  std::ofstream openWriteBinary(std::string_view filename) final;

  void init(int argc, char** argv);
  void shutdown();

private:
  std::filesystem::path baseDir{};
  std::filesystem::path userDir{};
  std::vector<std::filesystem::path> searchPaths{};
};

} // namespace motor

#endif //MOTOR_SHELLLOCAL_H
