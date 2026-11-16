#pragma once

#include "RegularPolygon.hpp"

template<Scalar T>
class Hexagon : public RegularPolygon<T, 6> {
    using Base = RegularPolygon<T, 6>;

public:
    using Base::Base;
    using Base::operator=;

    Hexagon() = default;
    Hexagon(const Hexagon&) = default;
    Hexagon(Hexagon&&) noexcept = default;
    Hexagon& operator=(const Hexagon&) = default;
    Hexagon& operator=(Hexagon&&) noexcept = default;
    ~Hexagon() override = default;

    [[nodiscard]] std::unique_ptr<Figure<T>> clone() const override {
        return std::make_unique<Hexagon<T>>(*this);
    }

    [[nodiscard]] std::string_view type() const override {
        return "Hexagon";
    }
};
