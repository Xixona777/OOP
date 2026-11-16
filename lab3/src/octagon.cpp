#include "octagon.hpp"

#include <istream>
#include <ostream>
#include <stdexcept>

namespace lab3 {

Octagon::Octagon() : Octagon(detail::regularPolygon<vertexCount>()) {}

Octagon::Octagon(const Vertices& vertices) : vertices_(vertices) {
    if (!detail::isRegularPolygon(vertices_.data(), vertexCount)) {
        throw std::invalid_argument("Некорректный правильный восьмиугольник");
    }
}

Point Octagon::center() const {
    return detail::geometricCenter(vertices_.data(), vertexCount);
}

void Octagon::read(std::istream& input) {
    Vertices temporary;
    for (Point& point : temporary) {
        if (!(input >> point.x >> point.y)) {
            return;
        }
    }
    if (!detail::isRegularPolygon(temporary.data(), vertexCount)) {
        throw std::invalid_argument("Некорректный правильный восьмиугольник");
    }
    vertices_ = temporary;
}

void Octagon::print(std::ostream& output) const {
    detail::writeVertices(output, vertices_.data(), vertexCount);
}

Octagon::operator double() const {
    return detail::polygonArea(vertices_.data(), vertexCount);
}

Figure* Octagon::clone() const {
    return new Octagon(*this);
}

bool Octagon::equals(const Figure& other) const {
    const auto* octagon = dynamic_cast<const Octagon*>(&other);
    return octagon != nullptr && *this == *octagon;
}

const char* Octagon::typeName() const noexcept {
    return "Octagon";
}

bool Octagon::operator==(const Octagon& other) const {
    return detail::equalVertices(vertices_.data(), other.vertices_.data(), vertexCount);
}

bool Octagon::operator!=(const Octagon& other) const {
    return !(*this == other);
}

}  // namespace lab3
