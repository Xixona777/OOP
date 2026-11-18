#pragma once

#include "battle_queue.h"
#include "dice.h"
#include "observer.h"
#include "world.h"

#include <memory>
#include <vector>

namespace lab7 {

class CombatSystem {
public:
    CombatSystem(World& world, BattleQueue& queue,
                 std::vector<std::shared_ptr<Observer>> observers, IDice& dice);

    bool processOne();
    void resolve(const BattleTask& task);

private:
    World& world_;
    BattleQueue& queue_;
    std::vector<std::shared_ptr<Observer>> observers_;
    IDice& dice_;
};

} // namespace lab7
