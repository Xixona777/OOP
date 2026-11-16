#pragma once

#include "figure.hpp"

#include <array>

namespace lab3 {

class Hexagon final : public Figure {
public:
    static constexpr std::size_t vertexCount = 6;
    using Vertices = std::array<Point, vertexCount>;

    Hexagon();
    explicit Hexagon(const Vertices& vertices);
    Hexagon(const Hexagon&) = default;
    Hexagon& operator=(const Hexagon&) = default;
    Hexagon(Hexagon&&) noexcept = default;
    Hexagon& operator=(Hexagon&&) noexcept = default;
    ~Hexagon() override = default;

    Point center() const override;
    void read(std::istream& input) override;
    void print(std::ostream& output) const override;
    operator double() const override;
    Figure* clone() const override;
    bool equals(const Figure& other) const override;
    const char* typeName() const noexcept override;

    bool operator==(const Hexagon& other) const;
    bool operator!=(const Hexagon& other) const;

private:
    Vertices vertices_;
};

}  // namespace lab3
