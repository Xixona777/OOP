#include "combat.h"

namespace lab7 {

CombatSystem::CombatSystem(World& world, BattleQueue& queue,
                           std::vector<std::shared_ptr<Observer>> observers,
                           IDice& dice)
    : world_(world), queue_(queue), observers_(std::move(observers)), dice_(dice) {}

bool CombatSystem::processOne() {
    auto task = queue_.pop();
    if (!task) {
        return false;
    }

    resolve(*task);
    queue_.complete(*task);
    return true;
}

void CombatSystem::resolve(const BattleTask& task) {
    const auto events = world_.applyBattle(task, dice_);
    for (const auto& event : events) {
        for (const auto& observer : observers_) {
            observer->onKill(event);
        }
    }
}

} // namespace lab7
