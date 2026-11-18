#include "observer.h"

#include <stdexcept>

namespace lab7 {

ConsoleObserver::ConsoleObserver(std::ostream& output, std::mutex& outputMutex)
    : output_(output), outputMutex_(outputMutex) {}

void ConsoleObserver::onKill(const KillEvent& event) {
    std::lock_guard lock(outputMutex_);
    output_ << "[KILL] " << toString(event.attackerType) << ' '
            << event.attackerName << " killed " << toString(event.victimType) << ' '
            << event.victimName << " at (" << event.victimPosition.x << ", "
            << event.victimPosition.y << ")\n";
}

FileObserver::FileObserver(const std::filesystem::path& path) : stream_(path) {
    if (!stream_) {
        throw std::runtime_error("Cannot open log file: " + path.string());
    }
}

void FileObserver::onKill(const KillEvent& event) {
    std::lock_guard lock(mutex_);
    stream_ << "[KILL] " << toString(event.attackerType) << ' '
            << event.attackerName << " killed " << toString(event.victimType) << ' '
            << event.victimName << " at (" << event.victimPosition.x << ", "
            << event.victimPosition.y << ")\n";
    stream_.flush();
}

} // namespace lab7
