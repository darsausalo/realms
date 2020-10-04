#ifndef MOTOR_FILE_WATCHER_HPP
#define MOTOR_FILE_WATCHER_HPP

#include <efsw/efsw.hpp>
#include <entt/entity/registry.hpp>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace motor {

namespace event {

enum class file_action { added, removed, modified, moved };

struct file_changed {
    file_action action;
    std::filesystem::path path;
    std::filesystem::path fullpath;
};

} // namespace event

class file_watcher : public efsw::FileWatchListener {
public:
    file_watcher(entt::registry& reg);

    void handleFileAction(efsw::WatchID watch_id, const std::string& dir,
                          const std::string& filename, efsw::Action action,
                          std::string oldFilename = "") override;

    void watch_directory(const std::filesystem::path& path);

private:
    entt::registry& reg;
    std::unique_ptr<efsw::FileWatcher> watcher;

    bool started{};
    std::unordered_map<efsw::WatchID, std::filesystem::path> paths;
};

} // namespace motor

#endif // MOTOR_FILE_WATCHER_HPP
