#pragma once

#include "RegularPolygon.hpp"

template<Scalar T>
class Triangle : public RegularPolygon<T, 3> {
    using Base = RegularPolygon<T, 3>;

public:
    using Base::Base;
    using Base::operator=;

    Triangle() = default;
    Triangle(const Triangle&) = default;
    Triangle(Triangle&&) noexcept = default;
    Triangle& operator=(const Triangle&) = default;
    Triangle& operator=(Triangle&&) noexcept = default;
    ~Triangle() override = default;

    [[nodiscard]] std::unique_ptr<Figure<T>> clone() const override {
        return std::make_unique<Triangle<T>>(*this);
    }

    [[nodiscard]] std::string_view type() const override {
        return "Triangle";
    }
};
