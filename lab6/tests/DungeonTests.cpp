#include "BattleVisitor.h"
#include "Dungeon.h"
#include "NpcFactory.h"
#include "Observers.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

class TempDirectoryTest : public testing::Test {
protected:
    void SetUp() override {
        static std::atomic<unsigned long> counter = 0;
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        directory_ = std::filesystem::temp_directory_path() /
                     ("dungeon_editor_tests_" + std::to_string(stamp) + "_" +
                      std::to_string(counter.fetch_add(1)));
        std::filesystem::create_directories(directory_);
    }

    void TearDown() override {
        std::error_code error;
        std::filesystem::remove_all(directory_, error);
    }

    std::filesystem::path file(const std::string& name) const {
        return directory_ / name;
    }

    void write(const std::filesystem::path& path, const std::string& content) const {
        std::ofstream output(path, std::ios::trunc);
        ASSERT_TRUE(output.is_open());
        output << content;
    }

    std::string read(const std::filesystem::path& path) const {
        std::ifstream input(path);
        return std::string(std::istreambuf_iterator<char>(input),
                           std::istreambuf_iterator<char>());
    }

    std::filesystem::path directory_;
};

class CountingObserver final : public IObserver {
public:
    void onKill(const Npc&, const Npc&) override {
        ++count;
    }

    int count = 0;
};

class SizeCheckingObserver final : public IObserver {
public:
    explicit SizeCheckingObserver(const Dungeon& dungeon) : dungeon_(dungeon) {}

    void onKill(const Npc&, const Npc&) override {
        sizesAtNotification.push_back(dungeon_.size());
    }

    std::vector<std::size_t> sizesAtNotification;

private:
    const Dungeon& dungeon_;
};

TEST(FactoryTest, CreatesElf) {
    NpcFactory factory;
    auto npc = factory.create("Elf", "Legolas", 10.0, 20.0);
    ASSERT_NE(dynamic_cast<Elf*>(npc.get()), nullptr);
    EXPECT_EQ(npc->type(), NpcType::Elf);
    EXPECT_EQ(npc->name(), "Legolas");
}

TEST(FactoryTest, CreatesDragon) {
    NpcFactory factory;
    auto npc = factory.create(NpcType::Dragon, "Smaug", 10.0, 20.0);
    ASSERT_NE(dynamic_cast<Dragon*>(npc.get()), nullptr);
    EXPECT_EQ(npc->type(), NpcType::Dragon);
}

TEST(FactoryTest, CreatesDruid) {
    NpcFactory factory;
    auto npc = factory.create("Druid", "Radagast", 10.0, 20.0);
    ASSERT_NE(dynamic_cast<Druid*>(npc.get()), nullptr);
    EXPECT_EQ(npc->type(), NpcType::Druid);
}

TEST(FactoryTest, RejectsUnknownType) {
    NpcFactory factory;
    EXPECT_THROW(factory.create("Orc", "Azog", 0.0, 0.0), std::invalid_argument);
    EXPECT_THROW(factory.create(static_cast<NpcType>(99), "Unknown", 0.0, 0.0),
                 std::invalid_argument);
}

TEST(NpcTest, AllowsMapBoundaries) {
    Dungeon dungeon;
    EXPECT_NO_THROW(dungeon.add(NpcType::Elf, "SouthWest", 0.0, 0.0));
    EXPECT_NO_THROW(dungeon.add(NpcType::Dragon, "NorthEast", 500.0, 500.0));
    EXPECT_NO_THROW(dungeon.add(NpcType::Druid, "NorthWest", 0.0, 500.0));
    EXPECT_EQ(dungeon.size(), 3U);
}

TEST(NpcTest, RejectsCoordinatesOutsideMap) {
    NpcFactory factory;
    EXPECT_THROW(factory.create("Elf", "NegativeX", -0.001, 0.0), std::invalid_argument);
    EXPECT_THROW(factory.create("Elf", "NegativeY", 0.0, -1.0), std::invalid_argument);
    EXPECT_THROW(factory.create("Elf", "LargeX", 500.001, 0.0), std::invalid_argument);
    EXPECT_THROW(factory.create("Elf", "LargeY", 0.0, 501.0), std::invalid_argument);
}

TEST(NpcTest, RejectsNonFiniteCoordinates) {
    NpcFactory factory;
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();
    EXPECT_THROW(factory.create("Elf", "Nan", nan, 0.0), std::invalid_argument);
    EXPECT_THROW(factory.create("Elf", "Infinity", 0.0, infinity), std::invalid_argument);
}

TEST(NpcTest, ComputesEuclideanDistance) {
    Elf first("First", 0.0, 0.0);
    Dragon second("Second", 3.0, 4.0);
    EXPECT_DOUBLE_EQ(first.distanceTo(second), 5.0);
    EXPECT_DOUBLE_EQ(second.distanceTo(first), 5.0);
}

TEST(DungeonTest, RejectsDuplicateNames) {
    Dungeon dungeon;
    dungeon.add(NpcType::Elf, "SameName", 1.0, 1.0);
    EXPECT_THROW(dungeon.add(NpcType::Dragon, "SameName", 2.0, 2.0),
                 std::invalid_argument);
    EXPECT_EQ(dungeon.size(), 1U);
}

TEST(DungeonTest, PrintsNpcList) {
    Dungeon dungeon;
    dungeon.add(NpcType::Elf, "Legolas", 10.5, 20.0);
    dungeon.add(NpcType::Dragon, "Smaug", 30.0, 40.0);
    std::ostringstream output;
    dungeon.print(output);
    EXPECT_EQ(output.str(), "Elf Legolas 10.5 20\nDragon Smaug 30 40\n");
}

TEST(DungeonTest, PrintsEmptyList) {
    Dungeon dungeon;
    std::ostringstream output;
    dungeon.print(output);
    EXPECT_EQ(output.str(), "Dungeon is empty\n");
}

TEST_F(TempDirectoryTest, SaveLoadRoundTripRestoresConcreteTypes) {
    const auto path = file("roundtrip.txt");
    Dungeon original;
    original.add(NpcType::Elf, "Legolas", 0.125, 500.0);
    original.add(NpcType::Dragon, "Smaug", 250.0, 300.0);
    original.add(NpcType::Druid, "Radagast", 499.0, 0.0);
    original.save(path);

    Dungeon loaded;
    loaded.load(path);
    ASSERT_EQ(loaded.size(), 3U);
    EXPECT_NE(dynamic_cast<Elf*>(loaded.npcs()[0].get()), nullptr);
    EXPECT_NE(dynamic_cast<Dragon*>(loaded.npcs()[1].get()), nullptr);
    EXPECT_NE(dynamic_cast<Druid*>(loaded.npcs()[2].get()), nullptr);
    EXPECT_EQ(loaded.npcs()[0]->name(), "Legolas");
    EXPECT_DOUBLE_EQ(loaded.npcs()[0]->x(), 0.125);
    EXPECT_DOUBLE_EQ(loaded.npcs()[0]->y(), 500.0);
}

TEST_F(TempDirectoryTest, SaveLoadSupportsEmptyDungeon) {
    const auto path = file("empty.txt");
    Dungeon original;
    original.save(path);
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(read(path).empty());

    Dungeon loaded;
    loaded.add(NpcType::Elf, "Temporary", 1.0, 1.0);
    loaded.load(path);
    EXPECT_TRUE(loaded.empty());
}

TEST_F(TempDirectoryTest, RejectsCorruptedFiles) {
    const auto path = file("corrupted.txt");
    const std::vector<std::string> invalidContents = {
        "Orc Azog 1 2\n",
        "Elf OutOfMap 501 2\n",
        "Elf Negative -1 2\n",
        "Elf Broken 10\n",
        "Elf Extra 1 2 unexpected\n",
        "Elf Same 1 2\nDragon Same 3 4\n",
    };

    for (const auto& content : invalidContents) {
        write(path, content);
        Dungeon dungeon;
        EXPECT_THROW(dungeon.load(path), std::runtime_error) << content;
    }
}

TEST_F(TempDirectoryTest, FailedLoadDoesNotPartiallyReplaceDungeon) {
    const auto path = file("partial.txt");
    write(path, "Dragon NewDragon 1 1\nElf Invalid 999 1\n");
    Dungeon dungeon;
    dungeon.add(NpcType::Druid, "Original", 10.0, 10.0);

    EXPECT_THROW(dungeon.load(path), std::runtime_error);
    ASSERT_EQ(dungeon.size(), 1U);
    EXPECT_EQ(dungeon.npcs()[0]->name(), "Original");
    EXPECT_NE(dynamic_cast<Druid*>(dungeon.npcs()[0].get()), nullptr);
}

TEST_F(TempDirectoryTest, LoadReportsFileOpenError) {
    Dungeon dungeon;
    EXPECT_THROW(dungeon.load(file("missing.txt")), std::runtime_error);
}

TEST_F(TempDirectoryTest, SaveReportsFileOpenError) {
    Dungeon dungeon;
    EXPECT_THROW(dungeon.save(directory_), std::runtime_error);
}

TEST(BattleTest, KillsAtExactRangeBoundary) {
    Dungeon dungeon;
    dungeon.add(NpcType::Dragon, "Smaug", 0.0, 0.0);
    dungeon.add(NpcType::Elf, "Legolas", 3.0, 4.0);
    EXPECT_EQ(dungeon.fight(5.0), 1U);
    ASSERT_EQ(dungeon.size(), 1U);
    EXPECT_EQ(dungeon.npcs()[0]->name(), "Smaug");
}

TEST(BattleTest, DragonKillsElf) {
    Dungeon dungeon;
    dungeon.add(NpcType::Dragon, "Smaug", 0.0, 0.0);
    dungeon.add(NpcType::Elf, "Legolas", 1.0, 0.0);
    EXPECT_EQ(dungeon.fight(1.0), 1U);
    ASSERT_EQ(dungeon.size(), 1U);
    EXPECT_EQ(dungeon.npcs()[0]->type(), NpcType::Dragon);
}

TEST(BattleTest, ElfKillsDruid) {
    Dungeon dungeon;
    dungeon.add(NpcType::Elf, "Legolas", 0.0, 0.0);
    dungeon.add(NpcType::Druid, "Radagast", 1.0, 0.0);
    EXPECT_EQ(dungeon.fight(1.0), 1U);
    ASSERT_EQ(dungeon.size(), 1U);
    EXPECT_EQ(dungeon.npcs()[0]->type(), NpcType::Elf);
}

TEST(BattleTest, DruidKillsDragon) {
    Dungeon dungeon;
    dungeon.add(NpcType::Druid, "Radagast", 0.0, 0.0);
    dungeon.add(NpcType::Dragon, "Smaug", 1.0, 0.0);
    EXPECT_EQ(dungeon.fight(1.0), 1U);
    ASSERT_EQ(dungeon.size(), 1U);
    EXPECT_EQ(dungeon.npcs()[0]->type(), NpcType::Druid);
}

TEST(BattleTest, InvalidAttackCombinationsDoNotKill) {
    BattleVisitor visitor;
    Elf elf("Elf", 0.0, 0.0);
    Elf otherElf("OtherElf", 0.0, 0.0);
    Dragon dragon("Dragon", 0.0, 0.0);
    Dragon otherDragon("OtherDragon", 0.0, 0.0);
    Druid druid("Druid", 0.0, 0.0);
    Druid otherDruid("OtherDruid", 0.0, 0.0);

    EXPECT_FALSE(visitor.canKill(elf, otherElf));
    EXPECT_FALSE(visitor.canKill(dragon, otherDragon));
    EXPECT_FALSE(visitor.canKill(druid, otherDruid));
    EXPECT_FALSE(visitor.canKill(elf, dragon));
    EXPECT_FALSE(visitor.canKill(dragon, druid));
    EXPECT_FALSE(visitor.canKill(druid, elf));
}

TEST(BattleTest, SameTypeNpcDoNotKillEachOther) {
    Dungeon dungeon;
    dungeon.add(NpcType::Elf, "First", 0.0, 0.0);
    dungeon.add(NpcType::Elf, "Second", 0.0, 0.0);
    EXPECT_EQ(dungeon.fight(0.0), 0U);
    EXPECT_EQ(dungeon.size(), 2U);
}

TEST(BattleTest, NoFightOutsideRange) {
    Dungeon dungeon;
    dungeon.add(NpcType::Dragon, "Smaug", 0.0, 0.0);
    dungeon.add(NpcType::Elf, "Legolas", 3.0, 4.0);
    EXPECT_EQ(dungeon.fight(std::nextafter(5.0, 0.0)), 0U);
    EXPECT_EQ(dungeon.size(), 2U);
}

TEST(BattleTest, RemovesDeadNpcAfterProcessingPairs) {
    Dungeon dungeon;
    dungeon.add(NpcType::Dragon, "Smaug", 0.0, 0.0);
    dungeon.add(NpcType::Elf, "Legolas", 1.0, 0.0);
    auto observer = std::make_shared<SizeCheckingObserver>(dungeon);
    dungeon.addObserver(observer);

    EXPECT_EQ(dungeon.fight(1.0), 1U);
    ASSERT_EQ(observer->sizesAtNotification.size(), 1U);
    EXPECT_EQ(observer->sizesAtNotification[0], 2U);
    EXPECT_EQ(dungeon.size(), 1U);
}

TEST(BattleTest, ProcessesEachPairOnlyOnce) {
    Dungeon dungeon;
    dungeon.add(NpcType::Dragon, "Smaug", 0.0, 0.0);
    dungeon.add(NpcType::Elf, "Legolas", 1.0, 0.0);
    auto observer = std::make_shared<CountingObserver>();
    dungeon.addObserver(observer);

    EXPECT_EQ(dungeon.fight(10.0), 1U);
    EXPECT_EQ(observer->count, 1);
}

TEST(BattleTest, RejectsInvalidRange) {
    Dungeon dungeon;
    EXPECT_THROW(dungeon.fight(-1.0), std::invalid_argument);
    EXPECT_THROW(dungeon.fight(std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
}

TEST_F(TempDirectoryTest, ConsoleAndFileObserversReceiveNotifications) {
    const auto logPath = file("log.txt");
    std::ostringstream console;
    auto consoleObserver = std::make_shared<ConsoleObserver>(console);
    auto fileObserver = std::make_shared<FileObserver>(logPath);
    Dungeon dungeon;
    dungeon.addObserver(consoleObserver);
    dungeon.addObserver(fileObserver);
    dungeon.add(NpcType::Dragon, "Smaug", 0.0, 0.0);
    dungeon.add(NpcType::Elf, "Legolas", 1.0, 1.0);

    EXPECT_EQ(dungeon.fight(2.0), 1U);
    const std::string event = "Dragon Smaug killed Elf Legolas\n";
    EXPECT_EQ(console.str(), event);
    EXPECT_TRUE(std::filesystem::exists(logPath));
    EXPECT_EQ(read(logPath), event);
}

TEST_F(TempDirectoryTest, FileObserverAppendsWithoutLosingExistingData) {
    const auto logPath = file("log.txt");
    write(logPath, "previous event\n");
    auto fileObserver = std::make_shared<FileObserver>(logPath);
    Dungeon dungeon;
    dungeon.addObserver(fileObserver);
    dungeon.add(NpcType::Elf, "Legolas", 0.0, 0.0);
    dungeon.add(NpcType::Druid, "Radagast", 1.0, 0.0);

    dungeon.fight(1.0);
    EXPECT_EQ(read(logPath),
              "previous event\nElf Legolas killed Druid Radagast\n");
}

TEST(ObserverTest, UnsubscribedObserverStopsReceivingNotifications) {
    std::ostringstream console;
    auto observer = std::make_shared<ConsoleObserver>(console);
    Dungeon dungeon;
    dungeon.addObserver(observer);
    dungeon.removeObserver(observer);
    dungeon.add(NpcType::Dragon, "Smaug", 0.0, 0.0);
    dungeon.add(NpcType::Elf, "Legolas", 0.0, 0.0);

    dungeon.fight(0.0);
    EXPECT_TRUE(console.str().empty());
}

TEST(ObserverTest, ExpiredObserverDoesNotLeaveDanglingPointer) {
    std::ostringstream console;
    auto activeObserver = std::make_shared<ConsoleObserver>(console);
    Dungeon dungeon;
    dungeon.addObserver(activeObserver);
    {
        auto temporaryObserver = std::make_shared<CountingObserver>();
        dungeon.addObserver(temporaryObserver);
    }
    dungeon.add(NpcType::Druid, "Radagast", 0.0, 0.0);
    dungeon.add(NpcType::Dragon, "Smaug", 0.0, 0.0);

    EXPECT_NO_THROW(dungeon.fight(0.0));
    EXPECT_EQ(console.str(), "Druid Radagast killed Dragon Smaug\n");
}

} // namespace
