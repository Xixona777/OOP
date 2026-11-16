#include "pentagon.hpp"

#include <istream>
#include <ostream>
#include <stdexcept>

namespace lab3 {

Pentagon::Pentagon() : Pentagon(detail::regularPolygon<vertexCount>()) {}

Pentagon::Pentagon(const Vertices& vertices) : vertices_(vertices) {
    if (!detail::isRegularPolygon(vertices_.data(), vertexCount)) {
        throw std::invalid_argument("Некорректный правильный пятиугольник");
    }
}

Point Pentagon::center() const {
    return detail::geometricCenter(vertices_.data(), vertexCount);
}

void Pentagon::read(std::istream& input) {
    Vertices temporary;
    for (Point& point : temporary) {
        if (!(input >> point.x >> point.y)) {
            return;
        }
    }
    if (!detail::isRegularPolygon(temporary.data(), vertexCount)) {
        throw std::invalid_argument("Некорректный правильный пятиугольник");
    }
    vertices_ = temporary;
}

void Pentagon::print(std::ostream& output) const {
    detail::writeVertices(output, vertices_.data(), vertexCount);
}

Pentagon::operator double() const {
    return detail::polygonArea(vertices_.data(), vertexCount);
}

Figure* Pentagon::clone() const {
    return new Pentagon(*this);
}

bool Pentagon::equals(const Figure& other) const {
    const auto* pentagon = dynamic_cast<const Pentagon*>(&other);
    return pentagon != nullptr && *this == *pentagon;
}

const char* Pentagon::typeName() const noexcept {
    return "Pentagon";
}

bool Pentagon::operator==(const Pentagon& other) const {
    return detail::equalVertices(vertices_.data(), other.vertices_.data(), vertexCount);
}

bool Pentagon::operator!=(const Pentagon& other) const {
    return !(*this == other);
}

}  // namespace lab3
