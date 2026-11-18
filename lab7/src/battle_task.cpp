#include "battle_task.h"

namespace lab7 {

BattleKey makeBattleKey(const BattleTask& task) {
    if (task.first->name() < task.second->name()) {
        return {task.first->name(), task.second->name()};
    }
    return {task.second->name(), task.first->name()};
}

} // namespace lab7
