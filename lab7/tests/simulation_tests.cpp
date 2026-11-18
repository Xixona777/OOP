#include "simulation.h"
#include "test_helpers.h"

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <sstream>
#include <thread>

using namespace lab7;
using namespace std::chrono_literals;

namespace {

SimulationConfig shortConfig() {
    SimulationConfig config;
    config.initialNpcCount = 10;
    config.duration = 80ms;
    config.movementInterval = 5ms;
    config.printInterval = 10ms;
    config.seed = 123;
    config.logFile.clear();
    config.printMaps = false;
    return config;
}

} // namespace

TEST(SimulationTest, ShortRunStopsJoinsAndPrintsSurvivors) {
    std::ostringstream output;
    Simulation simulation(shortConfig(), {}, nullptr, output);

    const auto start = std::chrono::steady_clock::now();
    simulation.run();
    const auto elapsed = std::chrono::steady_clock::now() - start;

    EXPECT_GE(elapsed, 60ms);
    EXPECT_LT(elapsed, 2s);
    EXPECT_TRUE(simulation.battleQueue().isStopped());
    EXPECT_NE(output.str().find("Survivors:"), std::string::npos);
    EXPECT_LE(simulation.world().survivors().size(), 10);
}

TEST(SimulationTest, ExternalStopIsIdempotentAndRejectsNewTasks) {
    std::ostringstream output;
    Simulation simulation(shortConfig(), {}, nullptr, output);
    simulation.start();
    std::this_thread::sleep_for(25ms);
    simulation.stop();
    simulation.join();
    simulation.stop();
    simulation.join();

    auto robber = std::make_shared<Robber>("extraR", Position{1, 1});
    auto werewolf = std::make_shared<Werewolf>("extraW", Position{1, 2});
    EXPECT_FALSE(simulation.battleQueue().push({robber, werewolf}));
}

TEST(SimulationTest, DeterministicInjectedDiceWorksAcrossThreads) {
    auto config = shortConfig();
    config.duration = 50ms;
    config.movementEnabled = false;
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 55});
    std::ostringstream output;
    Simulation simulation(config, {robber, werewolf},
                          std::make_unique<SequenceDice>(std::vector<int>{6, 1, 1, 6}),
                          output);

    simulation.run();

    EXPECT_EQ(simulation.world().survivors().size(), 1);
    const auto snapshots = simulation.world().snapshots();
    EXPECT_EQ(snapshots[0].alive + snapshots[1].alive, 1);
    EXPECT_NE(output.str().find("[KILL]"), std::string::npos);
}
