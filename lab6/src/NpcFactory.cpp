#include "NpcFactory.h"

#include <stdexcept>

std::unique_ptr<Npc> NpcFactory::create(NpcType type, const std::string& name,
                                        double x, double y) const {
    switch (type) {
    case NpcType::Elf:
        return std::make_unique<Elf>(name, x, y);
    case NpcType::Dragon:
        return std::make_unique<Dragon>(name, x, y);
    case NpcType::Druid:
        return std::make_unique<Druid>(name, x, y);
    }
    throw std::invalid_argument("Unknown NPC type");
}

std::unique_ptr<Npc> NpcFactory::create(std::string_view type,
                                        const std::string& name,
                                        double x, double y) const {
    if (type == "Elf") {
        return create(NpcType::Elf, name, x, y);
    }
    if (type == "Dragon") {
        return create(NpcType::Dragon, name, x, y);
    }
    if (type == "Druid") {
        return create(NpcType::Druid, name, x, y);
    }
    throw std::invalid_argument("Unknown NPC type: " + std::string(type));
}
