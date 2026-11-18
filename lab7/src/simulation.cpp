#include "simulation.h"

#include "combat.h"
#include "map_renderer.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace lab7 {

Simulation::Simulation(SimulationConfig config,
                       std::vector<World::NpcPtr> initialNpcs,
                       std::unique_ptr<IDice> dice, std::ostream& output)
    : config_(std::move(config)),
      world_(initialNpcs.empty()
                 ? NPCFactory::createRandomPopulation(config_.initialNpcCount,
                                                      config_.seed)
                 : std::move(initialNpcs)),
      output_(output), dice_(std::move(dice)) {
    if (config_.duration < std::chrono::milliseconds::zero() ||
        config_.movementInterval <= std::chrono::milliseconds::zero() ||
        config_.printInterval <= std::chrono::milliseconds::zero()) {
        throw std::invalid_argument("Simulation durations must be positive");
    }

    observers_.push_back(std::make_shared<ConsoleObserver>(output_, outputMutex_));
    if (!config_.logFile.empty()) {
        observers_.push_back(std::make_shared<FileObserver>(config_.logFile));
    }
}

Simulation::~Simulation() {
    stop();
    join();
}

void Simulation::start() {
    if (started_) {
        throw std::logic_error("Simulation can only be started once");
    }
    started_ = true;
    stopRequested_.store(false);

    combatThread_ = std::thread([this] { combatLoop(); });
    try {
        movementThread_ = std::thread([this] { movementLoop(); });
    } catch (...) {
        stop();
        if (combatThread_.joinable()) {
            combatThread_.join();
        }
        throw;
    }
}

void Simulation::stop() {
    stopRequested_.store(true);
    battleQueue_.stop();
    stopCondition_.notify_all();
}

void Simulation::join() {
    if (movementThread_.joinable()) {
        movementThread_.join();
    }
    if (combatThread_.joinable()) {
        combatThread_.join();
    }
}

void Simulation::run() {
    start();
    const auto startTime = std::chrono::steady_clock::now();
    const auto deadline = startTime + config_.duration;
    auto nextPrint = startTime;

    while (!stopRequested_.load()) {
        const auto now = std::chrono::steady_clock::now();
        if (now >= deadline) {
            break;
        }
        if (config_.printMaps && now >= nextPrint) {
            printMap();
            nextPrint = now + config_.printInterval;
        }

        const auto nextWake = config_.printMaps
                                  ? std::min(nextPrint, deadline)
                                  : deadline;
        waitUntil(nextWake);
    }

    stop();
    join();
    printSurvivors();
}

World& Simulation::world() noexcept {
    return world_;
}

const World& Simulation::world() const noexcept {
    return world_;
}

BattleQueue& Simulation::battleQueue() noexcept {
    return battleQueue_;
}

void Simulation::movementLoop() {
    std::mt19937 generator(config_.seed ^ 0x9E3779B9U);
    while (!stopRequested_.load()) {
        if (config_.movementEnabled) {
            world_.moveAll(generator);
        }
        for (const auto& task : world_.findBattleTasks()) {
            battleQueue_.push(task);
        }

        std::unique_lock lock(stopMutex_);
        stopCondition_.wait_for(lock, config_.movementInterval,
                                [this] { return stopRequested_.load(); });
    }
}

void Simulation::combatLoop() {
    RandomDice randomDice(config_.seed ^ 0x85EBCA6BU);
    IDice& selectedDice = dice_ ? *dice_ : randomDice;
    CombatSystem combat(world_, battleQueue_, observers_, selectedDice);
    while (combat.processOne()) {
    }
}

void Simulation::printMap() {
    const std::string map = MapRenderer::render(world_);
    std::lock_guard lock(outputMutex_);
    output_ << map << std::flush;
}

void Simulation::printSurvivors() {
    const std::string survivors = MapRenderer::renderSurvivors(world_);
    std::lock_guard lock(outputMutex_);
    output_ << survivors << std::flush;
}

void Simulation::waitUntil(std::chrono::steady_clock::time_point timePoint) {
    std::unique_lock lock(stopMutex_);
    stopCondition_.wait_until(lock, timePoint,
                              [this] { return stopRequested_.load(); });
}

} // namespace lab7
