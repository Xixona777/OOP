#include "Demo.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        demonstrateTemplateArrays();
        runInteractiveDemo();
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
