#pragma once

#include "npc.h"

#include <memory>
#include <string>
#include <utility>

namespace lab7 {

struct BattleTask {
    std::shared_ptr<NPC> first;
    std::shared_ptr<NPC> second;
};

using BattleKey = std::pair<std::string, std::string>;

[[nodiscard]] BattleKey makeBattleKey(const BattleTask& task);

} // namespace lab7
