#include <iostream>
#include <string>

#include "vowels.hpp"

int main() {
    std::string text;
    std::getline(std::cin, text);

    std::cout << countVowels(text) << '\n';

    return 0;
}
