#include "map_renderer.h"
#include "test_helpers.h"
#include "world.h"

#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <random>

using namespace lab7;

TEST(MovementTest, MovesExactlyTheTypeDistance) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    World world({robber});

    ASSERT_TRUE(world.moveNpc("R", Direction::Right));
    EXPECT_EQ(robber->position(), (Position{60, 50}));
    EXPECT_DOUBLE_EQ(distance(*robber, Robber("origin", {50, 50})), 10.0);
}

TEST(MovementTest, OrcAndWerewolfUseTheirOwnDistances) {
    auto orc = std::make_shared<Orc>("O", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 50});
    World world({orc, werewolf});

    ASSERT_TRUE(world.moveNpc("O", Direction::Up));
    ASSERT_TRUE(world.moveNpc("W", Direction::Left));
    EXPECT_EQ(orc->position(), (Position{50, 70}));
    EXPECT_EQ(werewolf->position(), (Position{10, 50}));
}

TEST(MovementTest, ClampsMovementAtMapBoundaries) {
    auto orc = std::make_shared<Orc>("O", Position{95, 100});
    World world({orc});

    ASSERT_TRUE(world.moveNpc("O", Direction::Right));
    EXPECT_EQ(orc->position(), (Position{100, 100}));
    ASSERT_TRUE(world.moveNpc("O", Direction::Up));
    EXPECT_EQ(orc->position(), (Position{100, 100}));
    ASSERT_TRUE(world.moveNpc("O", Direction::Left));
    ASSERT_TRUE(world.moveNpc("O", Direction::Left));
    ASSERT_TRUE(world.moveNpc("O", Direction::Left));
    ASSERT_TRUE(world.moveNpc("O", Direction::Left));
    ASSERT_TRUE(world.moveNpc("O", Direction::Left));
    EXPECT_EQ(orc->position(), (Position{0, 100}));
    ASSERT_TRUE(world.moveNpc("O", Direction::Left));
    EXPECT_EQ(orc->position(), (Position{0, 100}));
}

TEST(MovementTest, RandomMovementNeverLeavesMap) {
    auto werewolf = std::make_shared<Werewolf>("W", Position{0, 100});
    World world({werewolf});
    std::mt19937 generator(7);

    for (int step = 0; step < 1000; ++step) {
        world.moveAll(generator);
        EXPECT_GE(werewolf->position().x, 0);
        EXPECT_LE(werewolf->position().x, 100);
        EXPECT_GE(werewolf->position().y, 0);
        EXPECT_LE(werewolf->position().y, 100);
    }
}

TEST(MovementTest, DeadNpcDoesNotMove) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    World world({robber});
    robber->kill();
    std::mt19937 generator(42);

    EXPECT_FALSE(world.moveNpc("R", Direction::Right));
    world.moveAll(generator);
    EXPECT_EQ(robber->position(), (Position{50, 50}));
}

TEST(KillDistanceTest, RobberAndOrcBoundaryIsInclusive) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 60});
    auto orc = std::make_shared<Orc>("O", Position{50, 50});
    World inRange({robber, werewolf});
    World orcInRange({robber, orc});
    EXPECT_EQ(inRange.findBattleTasks().size(), 1);
    EXPECT_EQ(orcInRange.findBattleTasks().size(), 1);

    werewolf->setPosition({50, 61});
    orc->setPosition({50, 61});
    EXPECT_TRUE(inRange.findBattleTasks().empty());
    EXPECT_TRUE(orcInRange.findBattleTasks().empty());
}

TEST(KillDistanceTest, WerewolfBoundaryIsFive) {
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 50});
    auto robber = std::make_shared<Robber>("R", Position{50, 55});
    World inRange({werewolf, robber});
    SequenceDice inRangeDice({6, 1, 1, 6});
    const auto events = inRange.applyBattle({werewolf, robber}, inRangeDice);
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events.front().attackerName, "W");
    EXPECT_EQ(inRangeDice.rolls(), 4);

    auto distantWerewolf = std::make_shared<Werewolf>("W2", Position{50, 50});
    auto distantRobber = std::make_shared<Robber>("R2", Position{50, 56});
    World outOfRange({distantWerewolf, distantRobber});
    SequenceDice outOfRangeDice({1, 6});
    EXPECT_TRUE(
        outOfRange.applyBattle({distantWerewolf, distantRobber}, outOfRangeDice).empty());
    EXPECT_EQ(outOfRangeDice.rolls(), 2);
    EXPECT_TRUE(distantWerewolf->isAlive());
}

TEST(MapAndSurvivorsTest, DeadNpcsAreExcluded) {
    auto robber = std::make_shared<Robber>("DeadR", Position{1, 1});
    auto orc = std::make_shared<Orc>("LiveO", Position{2, 2});
    World world({robber, orc});
    robber->kill();

    const std::string map = MapRenderer::render(world);
    const std::string grid = map.substr(map.find('\n') + 1);
    EXPECT_EQ(grid.find('R'), std::string::npos);
    EXPECT_NE(grid.find('O'), std::string::npos);

    const auto survivors = world.survivors();
    ASSERT_EQ(survivors.size(), 1);
    EXPECT_EQ(survivors.front().name, "LiveO");
    const std::string list = MapRenderer::renderSurvivors(world);
    EXPECT_EQ(list.find("DeadR"), std::string::npos);
    EXPECT_NE(list.find("LiveO"), std::string::npos);
}
