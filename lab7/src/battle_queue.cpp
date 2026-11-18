#include "battle_queue.h"

#include <utility>

namespace lab7 {

bool BattleQueue::push(BattleTask task) {
    if (!task.first || !task.second || task.first == task.second) {
        return false;
    }

    const BattleKey key = makeBattleKey(task);
    {
        std::lock_guard lock(mutex_);
        if (stopped_ || activeTasks_.contains(key)) {
            return false;
        }
        activeTasks_.insert(key);
        tasks_.push_back(std::move(task));
    }
    condition_.notify_one();
    return true;
}

std::optional<BattleTask> BattleQueue::pop() {
    std::unique_lock lock(mutex_);
    condition_.wait(lock, [this] { return stopped_ || !tasks_.empty(); });
    if (tasks_.empty()) {
        return std::nullopt;
    }

    BattleTask task = std::move(tasks_.front());
    tasks_.pop_front();
    return task;
}

void BattleQueue::complete(const BattleTask& task) {
    std::lock_guard lock(mutex_);
    activeTasks_.erase(makeBattleKey(task));
}

void BattleQueue::stop() {
    {
        std::lock_guard lock(mutex_);
        stopped_ = true;
    }
    condition_.notify_all();
}

std::size_t BattleQueue::size() const {
    std::lock_guard lock(mutex_);
    return tasks_.size();
}

std::size_t BattleQueue::activeSize() const {
    std::lock_guard lock(mutex_);
    return activeTasks_.size();
}

bool BattleQueue::isStopped() const {
    std::lock_guard lock(mutex_);
    return stopped_;
}

} // namespace lab7
