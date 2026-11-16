#include "figure.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <istream>
#include <ostream>

namespace lab3 {

bool Figure::operator==(const Figure& other) const {
    return equals(other);
}

bool Figure::operator!=(const Figure& other) const {
    return !(*this == other);
}

std::istream& operator>>(std::istream& input, Figure& figure) {
    figure.read(input);
    return input;
}

std::ostream& operator<<(std::ostream& output, const Figure& figure) {
    figure.print(output);
    return output;
}

namespace detail {
namespace {

bool nearlyEqual(double lhs, double rhs, double tolerance = 1e-9) {
    const double scale = std::max({1.0, std::abs(lhs), std::abs(rhs)});
    return std::abs(lhs - rhs) <= tolerance * scale;
}

}  // namespace

Point geometricCenter(const Point* vertices, std::size_t count) {
    Point result;
    for (std::size_t i = 0; i < count; ++i) {
        result.x += vertices[i].x;
        result.y += vertices[i].y;
    }
    result.x /= static_cast<double>(count);
    result.y /= static_cast<double>(count);
    return result;
}

double polygonArea(const Point* vertices, std::size_t count) {
    double doubledArea = 0.0;
    for (std::size_t i = 0; i < count; ++i) {
        const Point& current = vertices[i];
        const Point& next = vertices[(i + 1) % count];
        doubledArea += current.x * next.y - next.x * current.y;
    }
    return std::abs(doubledArea) / 2.0;
}

bool isRegularPolygon(const Point* vertices, std::size_t count) {
    if (count < 3) {
        return false;
    }

    std::array<double, 8> squaredSides{};
    double maximumSide = 0.0;
    for (std::size_t i = 0; i < count; ++i) {
        if (!std::isfinite(vertices[i].x) || !std::isfinite(vertices[i].y)) {
            return false;
        }
        const Point& next = vertices[(i + 1) % count];
        const double dx = next.x - vertices[i].x;
        const double dy = next.y - vertices[i].y;
        squaredSides[i] = dx * dx + dy * dy;
        maximumSide = std::max(maximumSide, squaredSides[i]);
    }

    if (!std::isfinite(maximumSide) || maximumSide <= 0.0) {
        return false;
    }

    const double zeroTolerance = maximumSide * 1e-12;
    for (std::size_t i = 0; i < count; ++i) {
        if (squaredSides[i] <= zeroTolerance
            || !nearlyEqual(squaredSides[i], squaredSides[0], 1e-8)) {
            return false;
        }
    }

    int orientation = 0;
    for (std::size_t i = 0; i < count; ++i) {
        const Point& previous = vertices[(i + count - 1) % count];
        const Point& current = vertices[i];
        const Point& next = vertices[(i + 1) % count];
        const double firstX = current.x - previous.x;
        const double firstY = current.y - previous.y;
        const double secondX = next.x - current.x;
        const double secondY = next.y - current.y;
        const double cross = firstX * secondY - firstY * secondX;
        if (!std::isfinite(cross) || std::abs(cross) <= zeroTolerance) {
            return false;
        }
        const int currentOrientation = cross > 0.0 ? 1 : -1;
        if (orientation == 0) {
            orientation = currentOrientation;
        } else if (orientation != currentOrientation) {
            return false;
        }
    }

    double referenceAngle = 0.0;
    for (std::size_t i = 0; i < count; ++i) {
        const Point& previous = vertices[(i + count - 1) % count];
        const Point& current = vertices[i];
        const Point& next = vertices[(i + 1) % count];
        const double firstX = previous.x - current.x;
        const double firstY = previous.y - current.y;
        const double secondX = next.x - current.x;
        const double secondY = next.y - current.y;
        const double denominator = std::sqrt(
            (firstX * firstX + firstY * firstY)
            * (secondX * secondX + secondY * secondY));
        if (denominator <= 0.0 || !std::isfinite(denominator)) {
            return false;
        }
        const double angle = (firstX * secondX + firstY * secondY) / denominator;
        if (i == 0) {
            referenceAngle = angle;
        } else if (!nearlyEqual(angle, referenceAngle, 1e-8)) {
            return false;
        }
    }

    return true;
}

bool equalVertices(const Point* lhs, const Point* rhs, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
        if (!nearlyEqual(lhs[i].x, rhs[i].x)
            || !nearlyEqual(lhs[i].y, rhs[i].y)) {
            return false;
        }
    }
    return true;
}

void writeVertices(std::ostream& output, const Point* vertices, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
        if (i != 0) {
            output << ' ';
        }
        output << '(' << vertices[i].x << ", " << vertices[i].y << ')';
    }
}

}  // namespace detail
}  // namespace lab3
