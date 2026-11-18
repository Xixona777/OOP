#include "map_renderer.h"

#include <sstream>
#include <vector>

namespace lab7 {

std::string MapRenderer::render(const World& world) {
    std::vector<std::string> grid(World::mapHeight + 1,
                                  std::string(World::mapWidth + 1, '.'));
    for (const auto& npc : world.survivors()) {
        grid[World::mapHeight - npc.position.y][npc.position.x] = symbol(npc.type);
    }

    std::ostringstream output;
    output << "Map 100 x 100 (R=Robber, O=Orc, W=Werewolf, .=empty)\n";
    for (const auto& row : grid) {
        output << row << '\n';
    }
    return output.str();
}

std::string MapRenderer::renderSurvivors(const World& world) {
    const auto survivors = world.survivors();
    std::ostringstream output;
    output << "Survivors: " << survivors.size() << '\n';
    for (const auto& npc : survivors) {
        output << toString(npc.type) << ' ' << npc.name << " (" << npc.position.x
               << ", " << npc.position.y << ")\n";
    }
    return output.str();
}

} // namespace lab7
