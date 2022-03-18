#ifndef MOTOR_FILE_WATCHER_HPP
#define MOTOR_FILE_WATCHER_HPP

#include "core/internal_events.hpp"
#include <atomic_queue/atomic_queue.h>
#include <efsw/efsw.hpp>
#include <entt/signal/fwd.hpp>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace motor {

class file_watcher : public efsw::FileWatchListener {
    using file_changed_type = event::file_changed;
    using file_changed_ptr_type = std::unique_ptr<file_changed_type>;

public:
    file_watcher(entt::dispatcher& dispatcher);

    void handleFileAction(efsw::WatchID watch_id,
                          const std::string& dir,
                          const std::string& filename,
                          efsw::Action action,
                          std::string oldFilename = "") override;

    void watch_directory(const std::filesystem::path& path);

    void propogate_events();

private:
    static constexpr auto queue_capacity = 1024u;
    // static constexpr auto nil_file_changed = file_changed_type{};

    entt::dispatcher& dispatcher;
    std::unique_ptr<efsw::FileWatcher> watcher;

    bool started{};
    std::unordered_map<efsw::WatchID, std::filesystem::path> paths{};

    atomic_queue::AtomicQueueB2<file_changed_ptr_type> queue{queue_capacity};
};

} // namespace motor

#endif // MOTOR_FILE_WATCHER_HPP
