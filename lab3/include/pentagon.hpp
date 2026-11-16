#pragma once

#include "figure.hpp"

#include <array>

namespace lab3 {

class Pentagon final : public Figure {
public:
    static constexpr std::size_t vertexCount = 5;
    using Vertices = std::array<Point, vertexCount>;

    Pentagon();
    explicit Pentagon(const Vertices& vertices);
    Pentagon(const Pentagon&) = default;
    Pentagon& operator=(const Pentagon&) = default;
    Pentagon(Pentagon&&) noexcept = default;
    Pentagon& operator=(Pentagon&&) noexcept = default;
    ~Pentagon() override = default;

    Point center() const override;
    void read(std::istream& input) override;
    void print(std::ostream& output) const override;
    operator double() const override;
    Figure* clone() const override;
    bool equals(const Figure& other) const override;
    const char* typeName() const noexcept override;

    bool operator==(const Pentagon& other) const;
    bool operator!=(const Pentagon& other) const;

private:
    Vertices vertices_;
};

}  // namespace lab3
