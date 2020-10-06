#include "file_watcher.hpp"
#include "core/internal_events.hpp"
#include "motor/app/locator.hpp"
#include "motor/core/events.hpp"
#include "motor/core/files_service.hpp"
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

namespace motor {

file_watcher::file_watcher(entt::dispatcher& dispatcher)
    : dispatcher{dispatcher}, watcher{std::make_unique<efsw::FileWatcher>()} {
}

void file_watcher::watch_directory(const std::filesystem::path& path) {
    auto dir = path;
    dir.make_preferred();

    auto watch_id = watcher->addWatch(dir.string(), this, true);
    if (watch_id < 0) {
        spdlog::error("error watch: {} ({})", dir.string(), watch_id);
    } else {
        spdlog::debug("start watch: {}", dir.string());

        if (!started) {
            started = true;
            watcher->watch();
        }

        paths.emplace(watch_id, dir);
    }
}

void file_watcher::handleFileAction(efsw::WatchID watch_id,
                                    const std::string& dir,
                                    const std::string& filename,
                                    efsw::Action action,
                                    std::string oldFilename) {
    std::filesystem::path root_path;
    if (auto it = paths.find(watch_id); it != paths.end()) {
        root_path = it->second;
    } else {
        spdlog::error("failed to handle file watch: watch ID {} not found",
                      watch_id);
        return;
    }

    auto fullpath = std::filesystem::path(dir) / filename;

    std::error_code ec;
    auto path = std::filesystem::relative(fullpath, root_path, ec);
    if (ec) {
        spdlog::error("failed to handle file watch: {}", ec.message());
        return;
    }

    event::file_action f_action;
    switch (action) {
    case efsw::Actions::Add:
        f_action = event::file_action::added;
        break;
    case efsw::Actions::Delete:
        f_action = event::file_action::removed;
        break;
    case efsw::Actions::Modified:
        f_action = event::file_action::modified;
        break;
    case efsw::Actions::Moved:
        f_action = event::file_action::moved;
        break;
    }

    dispatcher.enqueue<event::file_changed>(
            {f_action, path.generic_string(), fullpath});
}

} // namespace motor
