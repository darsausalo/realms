/*
===============================================================================

    Frontier Source Code
    Copyright (C) 2020 Denis Potapov, Auriga Games.

    This file is subject to the terms and conditions defined in
    file 'LICENSE', which is part of the Frontier Source Code.

    Shell.cpp
    Game host shell - main context to operate between host and user-space.

===============================================================================
*/

#include "motor/Core/Shell.h"
#include "motor/Core/Logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define CR_TRACE mlog::trace("{}", __FUNCTION__);
#define CR_LOG(...)                                                            \
  {                                                                            \
    char buf[1024 * 2];                                                        \
    sprintf(buf, __VA_ARGS__);                                                 \
    auto n = strlen(buf) - 1;                                                  \
    if (n > 0 && buf[n] == '\n') buf[n] = '\0';                                \
    if (n > 1 && buf[n - 1] == '\n') buf[n] = '\0';                            \
    mlog::info(buf);                                                           \
  }
#define CR_ERROR(...)                                                          \
  {                                                                            \
    char buf[1024 * 2];                                                        \
    sprintf(buf, __VA_ARGS__);                                                 \
    auto n = strlen(buf) - 1;                                                  \
    if (n > 0 && buf[n] == '\n') buf[n] = '\0';                                \
    if (n > 1 && buf[n - 1] == '\n') buf[n] = '\0';                            \
    mlog::error(buf);                                                          \
  }

#include <cr.h>

namespace fs = std::filesystem;

namespace motor {

#ifdef MOTOR_HOST

Shell shellLocal{};
Shell* shell = &shellLocal;

void Shell::init(int argc, char** argv) {
  // TODO: init options

  // initialize logger
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_level(spdlog::level::debug);

  // TODO: use option for log path
  auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      MOTOR_DEFAULT_LOG_PATH, true);
  fileSink->set_level(spdlog::level::trace);
  std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};

  mlog::logger = set<spdlog::logger>("default", sinks.begin(), sinks.end());

  // TODO: init shell from cmd line options
  // TODO: real dataDir

  baseDir =
      (fs::current_path() / fs::path(MOTOR_DEFAULT_DATA_DIR)).generic_string();
  userDir =
      fs::temp_directory_path().generic_string(); // TODO: make real userPath

#ifdef MOTOR_NOINSTALL
  pluginFilename = MOTOR_DEPLOY_DIR "/" CR_PLUGIN("frontier");
#else
  pluginFilename =
      (fs::current_path() / CR_PLUGIN("frontier")).generic_string();
#endif

  mlog::info("initializing");
  mlog::info("baseDir: {}", baseDir);
  mlog::info("userDir: {}", userDir);
  mlog::info("pluginFilename: {}", pluginFilename);

  searchPaths.push_back(userDir);
  searchPaths.push_back(baseDir);
}

int Shell::run() {
  auto txt = readText("defs.yml");
  if (txt.has_value()) { mlog::info("txt: '{}'", txt.value()); }

  cr_plugin ctx;
  ctx.userdata = this;

  if (!cr_plugin_open(ctx, pluginFilename.c_str())) {
    mlog::error("failed to load: {}", pluginFilename);
    return -1;
  }
  cr_plugin_reload(ctx);
  if (ctx.failure) {
    mlog::error("failed to load plugin!");
    return -1;
  }

  int count = 2;
  while (count-- > 0) {
    mlog::info("update plugins");
    auto r = cr_plugin_update(ctx);
    if (ctx.failure) {
      mlog::error("failure in plugin");
      return r;
    }

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
  }

  mlog::info("close plugins");
  cr_plugin_close(ctx);

  return 0;
}

int Shell::run(int argc, char** argv) {
  shellLocal.init(argc, argv);
  return shellLocal.run();
}

#else

Shell* shell = nullptr;

#endif

std::optional<std::ifstream> Shell::openRead(std::string_view filename) {
  for (const auto& basePath : searchPaths) {
    auto path = basePath / filename;
    if (fs::exists(path)) { return std::ifstream(path); }
  }

  return {};
}

std::optional<std::filesystem::path>
Shell::findPath(std::string_view filename) {
  for (const auto& basePath : searchPaths) {
    auto path = basePath / filename;
    if (fs::exists(path)) { return path; }
  }
  return {};
}

std::optional<std::ofstream> Shell::openWrite(std::string_view filename) {
  for (const auto& basePath : searchPaths) {
    auto path = basePath / filename;
    if (fs::exists(path)) { return std::ofstream(path); }
  }

  return {};
}

std::optional<std::string> Shell::readText(std::string_view filename) {
  auto is = openRead(filename);
  if (is.has_value()) {
    std::string text;
    is.value().seekg(0, std::ios::end);
    text.reserve(is.value().tellg());
    is.value().seekg(0, std::ios::beg);

    text.assign(std::istreambuf_iterator<char>(is.value()),
                std::istreambuf_iterator<char>());

    return text;
  }
  return {};
}

} // namespace motor
