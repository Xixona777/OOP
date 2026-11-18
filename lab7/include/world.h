#pragma once

#include "battle_task.h"
#include "dice.h"
#include "npc.h"
#include "observer.h"

#include <memory>
#include <random>
#include <shared_mutex>
#include <string>
#include <vector>

namespace lab7 {

enum class Direction {
    Up,
    Right,
    Down,
    Left,
};

struct NpcSnapshot {
    NpcType type;
    std::string name;
    Position position;
    bool alive;
};

class World {
public:
    static constexpr int mapWidth = 100;
    static constexpr int mapHeight = 100;

    using NpcPtr = std::shared_ptr<NPC>;

    explicit World(std::vector<NpcPtr> npcs);

    void moveAll(std::mt19937& generator);
    bool moveNpc(const std::string& name, Direction direction);

    [[nodiscard]] std::vector<BattleTask> findBattleTasks() const;
    [[nodiscard]] std::vector<KillEvent> applyBattle(const BattleTask& task,
                                                     IDice& dice);

    [[nodiscard]] std::vector<NpcSnapshot> snapshots() const;
    [[nodiscard]] std::vector<NpcSnapshot> survivors() const;
    [[nodiscard]] std::vector<NpcPtr> npcPointers() const;

private:
    mutable std::shared_mutex mutex_;
    std::vector<NpcPtr> npcs_;
};

} // namespace lab7
