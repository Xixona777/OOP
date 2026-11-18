#pragma once

#include "battle_task.h"

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>
#include <set>

namespace lab7 {

class BattleQueue {
public:
    bool push(BattleTask task);
    [[nodiscard]] std::optional<BattleTask> pop();
    void complete(const BattleTask& task);
    void stop();

    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] std::size_t activeSize() const;
    [[nodiscard]] bool isStopped() const;

private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::deque<BattleTask> tasks_;
    std::set<BattleKey> activeTasks_;
    bool stopped_ = false;
};

} // namespace lab7
