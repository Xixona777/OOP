#include "npc_factory.h"

#include <random>
#include <stdexcept>
#include <string>

namespace lab7 {

NPCFactory::NpcPtr NPCFactory::create(NpcType type, std::string name,
                                      Position position) {
    switch (type) {
    case NpcType::Robber:
        return std::make_shared<Robber>(std::move(name), position);
    case NpcType::Orc:
        return std::make_shared<Orc>(std::move(name), position);
    case NpcType::Werewolf:
        return std::make_shared<Werewolf>(std::move(name), position);
    }
    throw std::invalid_argument("Unknown NPC type");
}

std::vector<NPCFactory::NpcPtr> NPCFactory::createRandomPopulation(
    std::size_t count, std::uint32_t seed) {
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> typeDistribution(0, 2);
    std::uniform_int_distribution<int> coordinateDistribution(0, 100);

    std::vector<NpcPtr> result;
    result.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        const auto type = static_cast<NpcType>(typeDistribution(generator));
        const Position position{coordinateDistribution(generator),
                                coordinateDistribution(generator)};
        result.push_back(create(type,
                                std::string(toString(type)) + "_" +
                                    std::to_string(index),
                                position));
    }
    return result;
}

} // namespace lab7
