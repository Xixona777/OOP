#include "combat.h"
#include "test_helpers.h"

#include <gtest/gtest.h>

#include <future>
#include <memory>

using namespace lab7;

TEST(CombatTest, AttackGreaterThanDefenseKillsAndNotifies) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 55});
    World world({robber, werewolf});
    BattleQueue queue;
    SequenceDice dice({6, 1, 1, 6});
    auto observer = std::make_shared<RecordingObserver>();
    CombatSystem combat(world, queue, {observer}, dice);

    combat.resolve({robber, werewolf});

    EXPECT_TRUE(robber->isAlive());
    EXPECT_FALSE(werewolf->isAlive());
    ASSERT_EQ(observer->events.size(), 1);
    EXPECT_EQ(observer->events.front().attackerName, "R");
    EXPECT_EQ(observer->events.front().victimName, "W");
}

TEST(CombatTest, EqualAttackAndDefenseMeansSuccessfulDefense) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 55});
    World world({robber, werewolf});
    SequenceDice dice({3, 3, 3, 3});

    const auto events = world.applyBattle({robber, werewolf}, dice);

    EXPECT_TRUE(events.empty());
    EXPECT_TRUE(robber->isAlive());
    EXPECT_TRUE(werewolf->isAlive());
    EXPECT_EQ(dice.rolls(), 4);
}

TEST(CombatTest, LowerAttackDoesNotKill) {
    auto orc = std::make_shared<Orc>("O", Position{50, 50});
    auto robber = std::make_shared<Robber>("R", Position{50, 55});
    World world({orc, robber});
    SequenceDice dice({1, 6});

    const auto events = world.applyBattle({orc, robber}, dice);

    EXPECT_TRUE(events.empty());
    EXPECT_TRUE(robber->isAlive());
}

TEST(CombatTest, RobberAndWerewolfCanKillEachOther) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 55});
    World world({robber, werewolf});
    SequenceDice dice({6, 1, 6, 1});

    const auto events = world.applyBattle({robber, werewolf}, dice);

    EXPECT_FALSE(robber->isAlive());
    EXPECT_FALSE(werewolf->isAlive());
    EXPECT_EQ(events.size(), 2);
    EXPECT_EQ(dice.rolls(), 4);
}

TEST(CombatTest, DeadParticipantSkipsBattleAndDice) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 55});
    World world({robber, werewolf});
    robber->kill();
    SequenceDice dice({});

    EXPECT_TRUE(world.applyBattle({robber, werewolf}, dice).empty());
    EXPECT_EQ(dice.rolls(), 0);
}

TEST(BattleQueueTest, QueuedTaskIsProcessed) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 55});
    World world({robber, werewolf});
    BattleQueue queue;
    SequenceDice dice({6, 1, 1, 6});
    CombatSystem combat(world, queue, {}, dice);

    ASSERT_TRUE(queue.push({robber, werewolf}));
    EXPECT_EQ(queue.size(), 1);
    EXPECT_TRUE(combat.processOne());

    EXPECT_FALSE(werewolf->isAlive());
    EXPECT_TRUE(robber->isAlive());
    EXPECT_EQ(queue.size(), 0);
    EXPECT_EQ(queue.activeSize(), 0);
}

TEST(BattleQueueTest, ActivePairCannotBeQueuedTwice) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 55});
    BattleQueue queue;
    const BattleTask task{robber, werewolf};

    ASSERT_TRUE(queue.push(task));
    EXPECT_FALSE(queue.push(task));
    EXPECT_FALSE(queue.push({werewolf, robber}));
    EXPECT_EQ(queue.size(), 1);
    EXPECT_EQ(queue.activeSize(), 1);

    const auto popped = queue.pop();
    ASSERT_TRUE(popped.has_value());
    EXPECT_EQ(queue.size(), 0);
    EXPECT_EQ(queue.activeSize(), 1);
    EXPECT_FALSE(queue.push(task));
    queue.complete(*popped);
    EXPECT_EQ(queue.activeSize(), 0);
    EXPECT_TRUE(queue.push(task));
}

TEST(BattleQueueTest, StopWakesWaitingConsumer) {
    BattleQueue queue;
    auto pending = std::async(std::launch::async,
                              [&queue] { return queue.pop(); });

    queue.stop();
    ASSERT_EQ(pending.wait_for(std::chrono::seconds(1)),
              std::future_status::ready);
    EXPECT_FALSE(pending.get().has_value());
    EXPECT_TRUE(queue.isStopped());
}

TEST(BattleQueueTest, StopDrainsPendingTaskThenReturnsNullopt) {
    auto robber = std::make_shared<Robber>("R", Position{50, 50});
    auto werewolf = std::make_shared<Werewolf>("W", Position{50, 55});
    BattleQueue queue;
    ASSERT_TRUE(queue.push({robber, werewolf}));
    queue.stop();

    EXPECT_TRUE(queue.pop().has_value());
    EXPECT_FALSE(queue.pop().has_value());
    EXPECT_FALSE(queue.push({robber, werewolf}));
}
