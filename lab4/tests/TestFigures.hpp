#pragma once

#include "Hexagon.hpp"
#include "Octagon.hpp"
#include "Triangle.hpp"

#include <array>
#include <cmath>

inline std::array<Point<double>, 3> trianglePoints() {
    return {{{0.0, 0.0}, {2.0, 0.0}, {1.0, std::sqrt(3.0)}}};
}

inline std::array<Point<double>, 6> hexagonPoints() {
    const double height = std::sqrt(3.0);
    return {{{2.0, 0.0}, {1.0, height}, {-1.0, height},
             {-2.0, 0.0}, {-1.0, -height}, {1.0, -height}}};
}

inline std::array<Point<double>, 8> octagonPoints() {
    const double diagonal = std::sqrt(0.5);
    return {{{1.0, 0.0}, {diagonal, diagonal}, {0.0, 1.0},
             {-diagonal, diagonal}, {-1.0, 0.0}, {-diagonal, -diagonal},
             {0.0, -1.0}, {diagonal, -diagonal}}};
}
