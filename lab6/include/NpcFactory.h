#pragma once

#include "Npc.h"

#include <memory>
#include <string>
#include <string_view>

class NpcFactory {
public:
    std::unique_ptr<Npc> create(NpcType type, const std::string& name,
                                double x, double y) const;
    std::unique_ptr<Npc> create(std::string_view type, const std::string& name,
                                double x, double y) const;
};
