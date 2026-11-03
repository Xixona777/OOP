#include "Three.hpp"

#include <exception>
#include <iostream>

int main() {
    const Three first("222");
    const Three second("1");

    std::cout << "first = " << first.toString() << '\n';
    std::cout << "second = " << second.toString() << '\n';
    std::cout << "sum = " << first.add(second).toString() << '\n';
    std::cout << "difference = " << first.subtract(second).toString() << '\n';
    std::cout << std::boolalpha;
    std::cout << "first > second: " << first.greater(second) << '\n';
    std::cout << "first == second: " << first.equals(second) << '\n';

    const Three copied = first.copy();
    std::cout << "copy = " << copied.toString() << '\n';

    try {
        second.subtract(first);
    } catch (const std::exception& error) {
        std::cout << "Expected subtraction error: " << error.what() << '\n';
    }

    return 0;
}
