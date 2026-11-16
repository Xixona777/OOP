#pragma once

#include "point.hpp"

#include <array>
#include <cmath>

namespace lab3::test {

template <std::size_t N>
std::array<Point, N> regularPolygon(double centerX = 0.0, double centerY = 0.0,
                                    double radius = 1.0, double rotation = 0.0) {
    constexpr double pi = 3.14159265358979323846;
    std::array<Point, N> vertices;
    for (std::size_t i = 0; i < N; ++i) {
        const double angle = rotation + 2.0 * pi * static_cast<double>(i) / static_cast<double>(N);
        vertices[i] = {centerX + radius * std::cos(angle),
                       centerY + radius * std::sin(angle)};
    }
    return vertices;
}

template <std::size_t N>
double regularPolygonArea(double radius = 1.0) {
    constexpr double pi = 3.14159265358979323846;
    return static_cast<double>(N) * radius * radius
        * std::sin(2.0 * pi / static_cast<double>(N)) / 2.0;
}

}  // namespace lab3::test
