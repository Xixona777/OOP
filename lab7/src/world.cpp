#include "world.h"

#include <algorithm>
#include <mutex>

namespace lab7 {

World::World(std::vector<NpcPtr> npcs) : npcs_(std::move(npcs)) {}

void World::moveAll(std::mt19937& generator) {
    std::uniform_int_distribution<int> directionDistribution(0, 3);
    std::lock_guard lock(mutex_);
    for (const auto& npc : npcs_) {
        if (!npc->isAlive()) {
            continue;
        }

        Position next = npc->position();
        const int step = npc->moveDistance();
        switch (static_cast<Direction>(directionDistribution(generator))) {
        case Direction::Up:
            next.y += step;
            break;
        case Direction::Right:
            next.x += step;
            break;
        case Direction::Down:
            next.y -= step;
            break;
        case Direction::Left:
            next.x -= step;
            break;
        }
        next.x = std::clamp(next.x, 0, mapWidth);
        next.y = std::clamp(next.y, 0, mapHeight);
        npc->setPosition(next);
    }
}

bool World::moveNpc(const std::string& name, Direction direction) {
    std::lock_guard lock(mutex_);
    const auto iterator = std::find_if(npcs_.begin(), npcs_.end(),
                                       [&name](const NpcPtr& npc) {
                                           return npc->name() == name;
                                       });
    if (iterator == npcs_.end() || !(*iterator)->isAlive()) {
        return false;
    }

    const auto& npc = *iterator;
    Position next = npc->position();
    const int step = npc->moveDistance();
    switch (direction) {
    case Direction::Up:
        next.y += step;
        break;
    case Direction::Right:
        next.x += step;
        break;
    case Direction::Down:
        next.y -= step;
        break;
    case Direction::Left:
        next.x -= step;
        break;
    }
    next.x = std::clamp(next.x, 0, mapWidth);
    next.y = std::clamp(next.y, 0, mapHeight);
    npc->setPosition(next);
    return true;
}

std::vector<BattleTask> World::findBattleTasks() const {
    std::shared_lock lock(mutex_);
    std::vector<BattleTask> result;
    for (std::size_t first = 0; first < npcs_.size(); ++first) {
        if (!npcs_[first]->isAlive()) {
            continue;
        }
        for (std::size_t second = first + 1; second < npcs_.size(); ++second) {
            if (!npcs_[second]->isAlive()) {
                continue;
            }

            const auto& attacker = *npcs_[first];
            const auto& defender = *npcs_[second];
            const bool firstAttacks =
                attacker.canKill(defender) &&
                distance(attacker, defender) <= attacker.killDistance();
            const bool secondAttacks =
                defender.canKill(attacker) &&
                distance(defender, attacker) <= defender.killDistance();
            if (firstAttacks || secondAttacks) {
                result.push_back({npcs_[first], npcs_[second]});
            }
        }
    }
    return result;
}

std::vector<KillEvent> World::applyBattle(const BattleTask& task, IDice& dice) {
    std::lock_guard lock(mutex_);
    std::vector<KillEvent> events;
    if (!task.first || !task.second || !task.first->isAlive() ||
        !task.second->isAlive()) {
        return events;
    }

    auto& first = *task.first;
    auto& second = *task.second;
    const bool firstAttacks =
        first.canKill(second) && distance(first, second) <= first.killDistance();
    const bool secondAttacks =
        second.canKill(first) && distance(second, first) <= second.killDistance();

    if (firstAttacks) {
        const int attack = dice.roll();
        const int defense = dice.roll();
        if (attack > defense) {
            second.kill();
            events.push_back({first.type(), first.name(), second.type(), second.name(),
                              second.position()});
        }
    }
    if (secondAttacks) {
        const int attack = dice.roll();
        const int defense = dice.roll();
        if (attack > defense) {
            first.kill();
            events.push_back({second.type(), second.name(), first.type(), first.name(),
                              first.position()});
        }
    }
    return events;
}

std::vector<NpcSnapshot> World::snapshots() const {
    std::shared_lock lock(mutex_);
    std::vector<NpcSnapshot> result;
    result.reserve(npcs_.size());
    for (const auto& npc : npcs_) {
        result.push_back({npc->type(), npc->name(), npc->position(), npc->isAlive()});
    }
    return result;
}

std::vector<NpcSnapshot> World::survivors() const {
    std::shared_lock lock(mutex_);
    std::vector<NpcSnapshot> result;
    for (const auto& npc : npcs_) {
        if (npc->isAlive()) {
            result.push_back({npc->type(), npc->name(), npc->position(), true});
        }
    }
    return result;
}

std::vector<World::NpcPtr> World::npcPointers() const {
    std::shared_lock lock(mutex_);
    return npcs_;
}

} // namespace lab7
