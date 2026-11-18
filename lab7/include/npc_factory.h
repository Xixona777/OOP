#pragma once

#include "npc.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace lab7 {

class NPCFactory {
public:
    using NpcPtr = std::shared_ptr<NPC>;

    [[nodiscard]] static NpcPtr create(NpcType type, std::string name,
                                       Position position);
    [[nodiscard]] static std::vector<NpcPtr> createRandomPopulation(
        std::size_t count, std::uint32_t seed);
};

} // namespace lab7
