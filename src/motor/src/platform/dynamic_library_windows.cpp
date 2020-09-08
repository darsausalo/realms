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

namespace motor {

template<typename TP>
std::time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() +
                                                        system_clock::now());
    return system_clock::to_time_t(sctp);
}

class dynamic_library::impl {
public:
    explicit impl(std::string_view name) noexcept
        : name{name}, original_path{(locator::files::ref().get_data_path() /
                                     MOTOR_MODS_DIR / name / name)} {
        original_pdb_path = original_path;
        original_pdb_path += ".pdb";
        original_pdb_path.make_preferred();

        original_path += ".dll";
        original_path.make_preferred();
    }

    bool exists() const { return std::filesystem::exists(original_path); }

    bool is_loaded() const { return handle; }

    bool is_changed() const {
        if (!is_loaded()) {
            return false;
        }
        std::error_code ec;
        auto new_timestamp =
                std::filesystem::last_write_time(original_path, ec);
        if (ec) {
            spdlog::error(ec.message());
            return false;
        }
        return new_timestamp > timestamp;
    }

    bool load() {
        handle = {};
        version++;

        std::error_code ec;
        timestamp = std::filesystem::last_write_time(original_path, ec);
        if (ec) {
            spdlog::error(ec.message());
            return false;
        }
        timestamp += std::chrono::seconds(3);

        std::filesystem::path target_path{fmt::format(
                "{}\\{}-{}.dll", locator::files::ref().get_base_path().string(),
                name, version)};
        std::filesystem::path target_pdb_path{fmt::format(
                "{}\\{}-{}.pdb", locator::files::ref().get_base_path().string(),
                name, version)};

        if (!try_copy_file(original_path, target_path, ec)) {
            spdlog::error("can't load \"{}\": {}", target_path.string(),
                          ec.message());
            return false;
        }
        if (!try_copy_file(original_pdb_path, target_pdb_path, ec)) {
            spdlog::error("can't load \"{}\": {}", target_pdb_path.string(),
                          ec.message());
            return false;
        }

        handle = LoadLibraryW(std::data(target_path.wstring()));
        return handle;
    }

    void unload() {
        if (handle) {
            FreeLibrary(handle);
        }
        handle = {};
    }

    void* get_symbol(std::string_view name) const {
        return GetProcAddress(handle, std::data(name));
    }

private:
    std::size_t version{};
    std::string_view name{};
    std::filesystem::path original_path{};
    std::filesystem::path original_pdb_path{};
    std::filesystem::file_time_type timestamp{};
    HMODULE handle{};

    bool try_copy_file(const std::filesystem::path& from,
                       const std::filesystem::path& to, std::error_code& ec) {
        auto success = false;
        std::size_t try_count = 0;
        for (std::size_t try_count = 0; try_count < 5 && !success;
             ++try_count) {
            if (!std::filesystem::copy_file(
                        from, to,
                        std::filesystem::copy_options::overwrite_existing,
                        ec)) {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for((try_count + 1) * 0.1s);
            } else {
                success = true;
            }
        }
        return success;
    }
};


dynamic_library::dynamic_library(std::string_view inname) noexcept
    : name{inname}, p{std::make_unique<dynamic_library::impl>(name)} {
}

dynamic_library::~dynamic_library() {
}

bool dynamic_library::exists() const {
    return p->exists();
}

bool dynamic_library::is_loaded() const {
    return p->is_loaded();
}

bool dynamic_library::is_changed() const {
    return p->is_changed();
}

bool dynamic_library::load() {
    return p->load();
}

void dynamic_library::unload() {
    p->unload();
}

void* dynamic_library::get_symbol(std::string_view name) const {
    return p->get_symbol(name);
}

} // namespace motor

#endif // defined(_WIN32) && !defines(WINDOWS_STORE)
