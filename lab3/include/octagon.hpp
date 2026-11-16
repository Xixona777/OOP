#pragma once

#include "figure.hpp"

#include <array>

namespace lab3 {

class Octagon final : public Figure {
public:
    static constexpr std::size_t vertexCount = 8;
    using Vertices = std::array<Point, vertexCount>;

    Octagon();
    explicit Octagon(const Vertices& vertices);
    Octagon(const Octagon&) = default;
    Octagon& operator=(const Octagon&) = default;
    Octagon(Octagon&&) noexcept = default;
    Octagon& operator=(Octagon&&) noexcept = default;
    ~Octagon() override = default;

    Point center() const override;
    void read(std::istream& input) override;
    void print(std::ostream& output) const override;
    operator double() const override;
    Figure* clone() const override;
    bool equals(const Figure& other) const override;
    const char* typeName() const noexcept override;

    bool operator==(const Octagon& other) const;
    bool operator!=(const Octagon& other) const;

private:
    Vertices vertices_;
};

}  // namespace lab3
