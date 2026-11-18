#include "npc_factory.h"
#include "world.h"

#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <set>
#include <string>

using namespace lab7;

TEST(NPCFactoryTest, CreatesAllConcreteTypes) {
    const auto robber = NPCFactory::create(NpcType::Robber, "R", {1, 2});
    const auto orc = NPCFactory::create(NpcType::Orc, "O", {3, 4});
    const auto werewolf = NPCFactory::create(NpcType::Werewolf, "W", {5, 6});

    EXPECT_NE(dynamic_cast<Robber*>(robber.get()), nullptr);
    EXPECT_NE(dynamic_cast<Orc*>(orc.get()), nullptr);
    EXPECT_NE(dynamic_cast<Werewolf*>(werewolf.get()), nullptr);
    EXPECT_EQ(robber->name(), "R");
    EXPECT_EQ(orc->position(), (Position{3, 4}));
    EXPECT_TRUE(werewolf->isAlive());
}

TEST(NPCParametersTest, MovementAndKillDistancesMatchVariant10) {
    const Robber robber("R", {0, 0});
    const Orc orc("O", {0, 0});
    const Werewolf werewolf("W", {0, 0});

    EXPECT_EQ(robber.moveDistance(), 10);
    EXPECT_EQ(robber.killDistance(), 10);
    EXPECT_EQ(orc.moveDistance(), 20);
    EXPECT_EQ(orc.killDistance(), 10);
    EXPECT_EQ(werewolf.moveDistance(), 40);
    EXPECT_EQ(werewolf.killDistance(), 5);
}

TEST(Variant10RulesTest, RobberKillsWerewolf) {
    const Robber robber("R", {0, 0});
    const Werewolf werewolf("W", {0, 0});
    EXPECT_TRUE(robber.canKill(werewolf));
}

TEST(Variant10RulesTest, WerewolfKillsRobber) {
    const Werewolf werewolf("W", {0, 0});
    const Robber robber("R", {0, 0});
    EXPECT_TRUE(werewolf.canKill(robber));
}

TEST(Variant10RulesTest, OrcKillsRobber) {
    const Orc orc("O", {0, 0});
    const Robber robber("R", {0, 0});
    EXPECT_TRUE(orc.canKill(robber));
}

TEST(Variant10RulesTest, AllOtherAttackCombinationsAreForbidden) {
    const Robber robber("R", {0, 0});
    const Orc orc("O", {0, 0});
    const Werewolf werewolf("W", {0, 0});
    const std::array<const NPC*, 3> npcs{&robber, &orc, &werewolf};

    for (const NPC* attacker : npcs) {
        for (const NPC* defender : npcs) {
            const bool expected =
                (attacker->type() == NpcType::Robber &&
                 defender->type() == NpcType::Werewolf) ||
                (attacker->type() == NpcType::Werewolf &&
                 defender->type() == NpcType::Robber) ||
                (attacker->type() == NpcType::Orc &&
                 defender->type() == NpcType::Robber);
            EXPECT_EQ(attacker->canKill(*defender), expected)
                << toString(attacker->type()) << " -> " << toString(defender->type());
        }
    }
}

TEST(InitialPopulationTest, CreatesFiftyUniqueNpcsInsideMap) {
    const auto population = NPCFactory::createRandomPopulation(50, 12345);
    std::set<std::string> names;
    std::set<NpcType> types;

    ASSERT_EQ(population.size(), 50);
    for (const auto& npc : population) {
        names.insert(npc->name());
        types.insert(npc->type());
        EXPECT_GE(npc->position().x, 0);
        EXPECT_LE(npc->position().x, World::mapWidth);
        EXPECT_GE(npc->position().y, 0);
        EXPECT_LE(npc->position().y, World::mapHeight);
        EXPECT_TRUE(npc->isAlive());
    }
    EXPECT_EQ(names.size(), 50);
    EXPECT_EQ(types.size(), 3);
}
