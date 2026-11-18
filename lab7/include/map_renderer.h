#pragma once

#include "world.h"

#include <string>

namespace lab7 {

class MapRenderer {
public:
    [[nodiscard]] static std::string render(const World& world);
    [[nodiscard]] static std::string renderSurvivors(const World& world);
};

} // namespace lab7
