#pragma once

#include "point.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <iosfwd>

namespace lab3 {

class Figure {
public:
    virtual ~Figure() = default;

    virtual Point center() const = 0;
    virtual void read(std::istream& input) = 0;
    virtual void print(std::ostream& output) const = 0;
    virtual operator double() const = 0;
    virtual Figure* clone() const = 0;
    virtual bool equals(const Figure& other) const = 0;
    virtual const char* typeName() const noexcept = 0;

    bool operator==(const Figure& other) const;
    bool operator!=(const Figure& other) const;
};

std::istream& operator>>(std::istream& input, Figure& figure);
std::ostream& operator<<(std::ostream& output, const Figure& figure);

namespace detail {

template <std::size_t N>
std::array<Point, N> regularPolygon() {
    constexpr double pi = 3.14159265358979323846;
    std::array<Point, N> vertices;
    for (std::size_t i = 0; i < N; ++i) {
        const double angle = 2.0 * pi * static_cast<double>(i) / static_cast<double>(N);
        vertices[i] = {std::cos(angle), std::sin(angle)};
    }
    return vertices;
}

Point geometricCenter(const Point* vertices, std::size_t count);
double polygonArea(const Point* vertices, std::size_t count);
bool isRegularPolygon(const Point* vertices, std::size_t count);
bool equalVertices(const Point* lhs, const Point* rhs, std::size_t count);
void writeVertices(std::ostream& output, const Point* vertices, std::size_t count);

}  // namespace detail
}  // namespace lab3
