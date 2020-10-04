#ifndef MOTOR_PROGRESS_H
#define MOTOR_PROGRESS_H

#include <atomic>
#include <mutex>
#include <string>

namespace motor {

class progress {
public:
    progress() noexcept = default;
    progress(progress&) = delete;
    progress(progress&&) = delete;

    progress& operator=(progress&) = delete;
    progress& operator=(progress&&) = delete;

    ~progress() noexcept = default;

    void update(int32_t v) noexcept { value.store(value); }

    void update(std::string_view s) {
        std::unique_lock{mutex};
        status = s;
    }

    void update(int32_t v, std::string_view s) {
        value.store(v);
        update(s);
    }

    void complete() noexcept { completed.store(true); }

    [[nodiscard]] bool is_completed() const noexcept { return completed; }

private:
    std::atomic<bool> completed{};
    std::atomic<int32_t> value{};
    std::mutex mutex{};
    std::string status{};
};

} // namespace motor

#endif // MOTOR_PROGRESS_H
