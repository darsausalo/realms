#include "motor/platform/dynamic_library.h"
#include "motor/services/files_service.h"
#include "motor/services/locator.h"
#include <chrono>
#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <thread>

#if defined(_WIN32) && !defined(WINDOWS_STORE)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#endif // defined(_WIN32) && !defined(WINDOWS_STORE)

namespace motor {

#if defined(_WIN32) && !defined(WINDOWS_STORE)

static void* load_dynamic_library(const std::filesystem::path& path) {
    return LoadLibraryW(std::data(path.wstring()));
}

static void unload_dynamic_library(void* handle) {
    FreeLibrary(reinterpret_cast<HMODULE>(handle));
}

static void* get_library_symbol(void* handle, std::string_view name) {
    return GetProcAddress(reinterpret_cast<HMODULE>(handle), std::data(name));
}

#else

#error "TODO: implement Linux and OSX"

#endif

static bool try_copy_file(const std::filesystem::path& from,
                          const std::filesystem::path& to,
                          std::error_code& ec) {
    auto success = false;
    std::size_t try_count = 0;
    for (std::size_t try_count = 0; try_count < 5 && !success; ++try_count) {
        if (!std::filesystem::copy_file(
                    from, to, std::filesystem::copy_options::overwrite_existing,
                    ec)) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for((try_count + 1) * 0.1s);
        } else {
            success = true;
        }
    }
    return success;
}

dynamic_library::dynamic_library(std::string_view name,
                                 const std::filesystem::path& dir) noexcept
    : name{name}, original_path{dir / name} {
    original_pdb_path = original_path;
    original_pdb_path += ".pdb";
    original_pdb_path.make_preferred();

    original_path += ".dll";
    original_path.make_preferred();
}

dynamic_library::~dynamic_library() {
}

bool dynamic_library::exists() const {
    return std::filesystem::exists(original_path);
}

bool dynamic_library::is_loaded() const {
    return handle;
}

bool dynamic_library::is_changed() const {
    if (!is_loaded()) {
        return false;
    }
    std::error_code ec;
    auto new_timestamp = std::filesystem::last_write_time(original_path, ec);
    if (ec) {
        spdlog::error(ec.message());
        return false;
    }
    return new_timestamp > timestamp;
}

bool dynamic_library::load() {
    handle = {};
    version++;

    std::error_code ec;
    timestamp = std::filesystem::last_write_time(original_path, ec);
    if (ec) {
        spdlog::error(ec.message());
        return false;
    }
    timestamp += std::chrono::seconds(3); // TODO: remove?!

    std::filesystem::path target_path{fmt::format(
            "{}-{}.dll",
            (locator::files::ref().get_base_path() / name).string(), version)};
    std::filesystem::path target_pdb_path{fmt::format(
            "{}-{}.pdb",
            (locator::files::ref().get_base_path() / name).string(), version)};

    spdlog::debug("copy: {} -> {}", original_path.string(),
                  target_path.string());
    if (!try_copy_file(original_path, target_path, ec)) {
        spdlog::error("can't load \"{}\": {}", target_path.string(),
                      ec.message());
        return false;
    }
    spdlog::debug("copy: {} -> {}", original_pdb_path.string(),
                  target_pdb_path.string());
    if (!try_copy_file(original_pdb_path, target_pdb_path, ec)) {
        spdlog::error("can't load \"{}\": {}", target_pdb_path.string(),
                      ec.message());
        return false;
    }

    handle = load_dynamic_library(target_path.wstring());
    return handle;
}

void dynamic_library::unload() {
    if (handle) {
        unload_dynamic_library(handle);
    }
    handle = {};
}

void* dynamic_library::get_symbol(std::string_view name) const {
    return get_library_symbol(handle, name);
}

} // namespace motor
