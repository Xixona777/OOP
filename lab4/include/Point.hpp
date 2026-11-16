#pragma once

#include "Scalar.hpp"

template<Scalar T>
struct Point {
    T x{};
    T y{};

    friend bool operator==(const Point&, const Point&) = default;
};
