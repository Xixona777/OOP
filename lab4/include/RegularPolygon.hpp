#pragma once

#include "Figure.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <istream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <typeinfo>
#include <utility>

template<Scalar T, std::size_t N>
class RegularPolygon : public Figure<T> {
public:
    RegularPolygon() {
        for (auto& vertex : vertices_) {
            vertex = std::make_unique<Point<T>>();
        }
    }

    explicit RegularPolygon(const std::array<Point<T>, N>& points)
        : vertices_(makeVertices(points)) {
        validate(points);
    }

    RegularPolygon(const RegularPolygon& other) {
        for (std::size_t i = 0; i < N; ++i) {
            vertices_[i] = std::make_unique<Point<T>>(*other.vertices_[i]);
        }
    }

    RegularPolygon(RegularPolygon&&) noexcept = default;

    RegularPolygon& operator=(const RegularPolygon& other) {
        if (this != &other) {
            Vertices copy;
            for (std::size_t i = 0; i < N; ++i) {
                copy[i] = std::make_unique<Point<T>>(*other.vertices_[i]);
            }
            vertices_.swap(copy);
        }
        return *this;
    }

    RegularPolygon& operator=(RegularPolygon&&) noexcept = default;
    ~RegularPolygon() override = default;

    [[nodiscard]] Point<double> center() const override {
        long double x = 0.0L;
        long double y = 0.0L;
        for (const auto& vertex : vertices_) {
            x += static_cast<long double>(vertex->x);
            y += static_cast<long double>(vertex->y);
        }
        return {static_cast<double>(x / N), static_cast<double>(y / N)};
    }

    void print(std::ostream& output) const override {
        for (std::size_t i = 0; i < N; ++i) {
            if (i != 0) {
                output << ' ';
            }
            output << '(' << vertices_[i]->x << ", " << vertices_[i]->y << ')';
        }
    }

    void read(std::istream& input) override {
        std::array<Point<T>, N> points{};
        for (auto& point : points) {
            input >> point.x >> point.y;
        }
        if (!input) {
            return;
        }

        try {
            validate(points);
        } catch (const std::invalid_argument&) {
            input.setstate(std::ios::failbit);
            return;
        }
        auto updated = makeVertices(points);
        vertices_.swap(updated);
    }

    [[nodiscard]] bool equals(const Figure<T>& other) const override {
        if (typeid(*this) != typeid(other)) {
            return false;
        }
        const auto& rhs = dynamic_cast<const RegularPolygon&>(other);
        for (std::size_t i = 0; i < N; ++i) {
            if (*vertices_[i] != *rhs.vertices_[i]) {
                return false;
            }
        }
        return true;
    }

    explicit operator double() const override {
        return static_cast<double>(area());
    }

    [[nodiscard]] long double area() const {
        long double doubledArea = 0.0L;
        for (std::size_t i = 0; i < N; ++i) {
            const auto& current = *vertices_[i];
            const auto& next = *vertices_[(i + 1) % N];
            doubledArea += static_cast<long double>(current.x) * next.y
                         - static_cast<long double>(next.x) * current.y;
        }
        return std::abs(doubledArea) / 2.0L;
    }

    Point<T>& vertex(std::size_t index) {
        return *vertices_.at(index);
    }

    const Point<T>& vertex(std::size_t index) const {
        return *vertices_.at(index);
    }

protected:
    static void validate(const std::array<Point<T>, N>& points) {
        long double doubledArea = 0.0L;
        for (std::size_t i = 0; i < N; ++i) {
            doubledArea += static_cast<long double>(points[i].x) * points[(i + 1) % N].y
                         - static_cast<long double>(points[(i + 1) % N].x) * points[i].y;
        }
        if (nearlyEqual(doubledArea, 0.0L)) {
            throw std::invalid_argument("Polygon is degenerate");
        }

        int orientation = 0;
        for (std::size_t i = 0; i < N; ++i) {
            const auto& a = points[i];
            const auto& b = points[(i + 1) % N];
            const auto& c = points[(i + 2) % N];
            const long double cross = static_cast<long double>(b.x - a.x) * (c.y - b.y)
                                    - static_cast<long double>(b.y - a.y) * (c.x - b.x);
            if (nearlyEqual(cross, 0.0L)) {
                throw std::invalid_argument("Polygon is not strictly convex");
            }
            const int currentOrientation = cross > 0.0L ? 1 : -1;
            if (orientation == 0) {
                orientation = currentOrientation;
            } else if (orientation != currentOrientation) {
                throw std::invalid_argument("Polygon is not convex");
            }
        }

        const long double side = squaredDistance(points[0], points[1]);
        const long double diagonal = squaredDistance(points[0], points[2]);
        if (nearlyEqual(side, 0.0L)) {
            throw std::invalid_argument("Polygon has repeated vertices");
        }
        for (std::size_t i = 0; i < N; ++i) {
            if (!nearlyEqual(squaredDistance(points[i], points[(i + 1) % N]), side)
                || !nearlyEqual(squaredDistance(points[i], points[(i + 2) % N]), diagonal)) {
                throw std::invalid_argument("Vertices do not form the required regular polygon");
            }
        }
    }

private:
    using Vertices = std::array<std::unique_ptr<Point<T>>, N>;

    static Vertices makeVertices(const std::array<Point<T>, N>& points) {
        Vertices result;
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = std::make_unique<Point<T>>(points[i]);
        }
        return result;
    }

    static long double squaredDistance(const Point<T>& a, const Point<T>& b) {
        const long double dx = static_cast<long double>(a.x) - b.x;
        const long double dy = static_cast<long double>(a.y) - b.y;
        return dx * dx + dy * dy;
    }

    static bool nearlyEqual(long double left, long double right) {
        const long double scale = std::max({1.0L, std::abs(left), std::abs(right)});
        return std::abs(left - right) <= 1e-9L * scale;
    }

    Vertices vertices_{};
};
