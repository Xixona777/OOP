#pragma once

#include "battle_queue.h"
#include "dice.h"
#include "npc_factory.h"
#include "observer.h"
#include "world.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

namespace lab7 {

struct SimulationConfig {
    std::size_t initialNpcCount = 50;
    std::chrono::milliseconds duration{30000};
    std::chrono::milliseconds movementInterval{100};
    std::chrono::milliseconds printInterval{1000};
    std::uint32_t seed = std::random_device{}();
    std::filesystem::path logFile = "log.txt";
    bool printMaps = true;
    bool movementEnabled = true;
};

class Simulation {
public:
    explicit Simulation(SimulationConfig config = {},
                        std::vector<World::NpcPtr> initialNpcs = {},
                        std::unique_ptr<IDice> dice = nullptr,
                        std::ostream& output = std::cout);
    ~Simulation();

    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;

    void start();
    void stop();
    void join();
    void run();

    [[nodiscard]] World& world() noexcept;
    [[nodiscard]] const World& world() const noexcept;
    [[nodiscard]] BattleQueue& battleQueue() noexcept;

private:
    void movementLoop();
    void combatLoop();
    void printMap();
    void printSurvivors();
    void waitUntil(std::chrono::steady_clock::time_point timePoint);

    SimulationConfig config_;
    World world_;
    BattleQueue battleQueue_;
    std::ostream& output_;
    std::mutex outputMutex_;
    std::vector<std::shared_ptr<Observer>> observers_;
    std::unique_ptr<IDice> dice_;

    std::atomic<bool> stopRequested_{false};
    std::mutex stopMutex_;
    std::condition_variable stopCondition_;
    std::thread movementThread_;
    std::thread combatThread_;
    bool started_ = false;
};

} // namespace lab7
