#pragma once

#include "Point.hpp"

#include <iosfwd>
#include <memory>
#include <string_view>

template<Scalar T>
class Figure {
public:
    virtual ~Figure() = default;

    [[nodiscard]] virtual Point<double> center() const = 0;
    [[nodiscard]] virtual std::unique_ptr<Figure<T>> clone() const = 0;
    [[nodiscard]] virtual std::string_view type() const = 0;
    virtual void print(std::ostream& output) const = 0;
    virtual void read(std::istream& input) = 0;
    [[nodiscard]] virtual bool equals(const Figure<T>& other) const = 0;
    virtual explicit operator double() const = 0;
};

template<Scalar T>
std::ostream& operator<<(std::ostream& output, const Figure<T>& figure) {
    figure.print(output);
    return output;
}

template<Scalar T>
std::istream& operator>>(std::istream& input, Figure<T>& figure) {
    figure.read(input);
    return input;
}

template<Scalar T>
bool operator==(const Figure<T>& left, const Figure<T>& right) {
    return left.equals(right);
}

template<Scalar T>
bool operator!=(const Figure<T>& left, const Figure<T>& right) {
    return !(left == right);
}
