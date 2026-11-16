#pragma once

#include "RegularPolygon.hpp"

template<Scalar T>
class Octagon : public RegularPolygon<T, 8> {
    using Base = RegularPolygon<T, 8>;

public:
    using Base::Base;
    using Base::operator=;

    Octagon() = default;
    Octagon(const Octagon&) = default;
    Octagon(Octagon&&) noexcept = default;
    Octagon& operator=(const Octagon&) = default;
    Octagon& operator=(Octagon&&) noexcept = default;
    ~Octagon() override = default;

    [[nodiscard]] std::unique_ptr<Figure<T>> clone() const override {
        return std::make_unique<Octagon<T>>(*this);
    }

    [[nodiscard]] std::string_view type() const override {
        return "Octagon";
    }
};
