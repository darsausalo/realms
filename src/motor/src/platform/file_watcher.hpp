#ifndef MOTOR_FILE_WATCHER_HPP
#define MOTOR_FILE_WATCHER_HPP

#include <efsw/efsw.hpp>
#include <entt/signal/fwd.hpp>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace motor {

class file_watcher : public efsw::FileWatchListener {
public:
    file_watcher(entt::dispatcher& dispatcher);

    void handleFileAction(efsw::WatchID watch_id, const std::string& dir,
                          const std::string& filename, efsw::Action action,
                          std::string oldFilename = "") override;

    void watch_directory(const std::filesystem::path& path);

private:
    entt::dispatcher& dispatcher;
    std::unique_ptr<efsw::FileWatcher> watcher;

    bool started{};
    std::unordered_map<efsw::WatchID, std::filesystem::path> paths;
};

} // namespace motor

#endif // MOTOR_FILE_WATCHER_HPP
