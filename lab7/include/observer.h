#pragma once

#include "npc.h"

#include <filesystem>
#include <fstream>
#include <mutex>
#include <ostream>
#include <string>

namespace lab7 {

struct KillEvent {
    NpcType attackerType;
    std::string attackerName;
    NpcType victimType;
    std::string victimName;
    Position victimPosition;
};

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onKill(const KillEvent& event) = 0;
};

class ConsoleObserver final : public Observer {
public:
    ConsoleObserver(std::ostream& output, std::mutex& outputMutex);

    void onKill(const KillEvent& event) override;

private:
    std::ostream& output_;
    std::mutex& outputMutex_;
};

class FileObserver final : public Observer {
public:
    explicit FileObserver(const std::filesystem::path& path);

    void onKill(const KillEvent& event) override;

private:
    std::ofstream stream_;
    std::mutex mutex_;
};

} // namespace lab7
