#include "hexagon.hpp"

#include <istream>
#include <ostream>
#include <stdexcept>

namespace lab3 {

Hexagon::Hexagon() : Hexagon(detail::regularPolygon<vertexCount>()) {}

Hexagon::Hexagon(const Vertices& vertices) : vertices_(vertices) {
    if (!detail::isRegularPolygon(vertices_.data(), vertexCount)) {
        throw std::invalid_argument("Некорректный правильный шестиугольник");
    }
}

Point Hexagon::center() const {
    return detail::geometricCenter(vertices_.data(), vertexCount);
}

void Hexagon::read(std::istream& input) {
    Vertices temporary;
    for (Point& point : temporary) {
        if (!(input >> point.x >> point.y)) {
            return;
        }
    }
    if (!detail::isRegularPolygon(temporary.data(), vertexCount)) {
        throw std::invalid_argument("Некорректный правильный шестиугольник");
    }
    vertices_ = temporary;
}

void Hexagon::print(std::ostream& output) const {
    detail::writeVertices(output, vertices_.data(), vertexCount);
}

Hexagon::operator double() const {
    return detail::polygonArea(vertices_.data(), vertexCount);
}

Figure* Hexagon::clone() const {
    return new Hexagon(*this);
}

bool Hexagon::equals(const Figure& other) const {
    const auto* hexagon = dynamic_cast<const Hexagon*>(&other);
    return hexagon != nullptr && *this == *hexagon;
}

const char* Hexagon::typeName() const noexcept {
    return "Hexagon";
}

bool Hexagon::operator==(const Hexagon& other) const {
    return detail::equalVertices(vertices_.data(), other.vertices_.data(), vertexCount);
}

bool Hexagon::operator!=(const Hexagon& other) const {
    return !(*this == other);
}

}  // namespace lab3
