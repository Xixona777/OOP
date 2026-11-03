#include "vowels.hpp"

std::size_t countVowels(const std::string& text) {
    std::size_t count = 0;

    for (const char ch : text) {
        switch (ch) {
            case 'a':
            case 'e':
            case 'i':
            case 'o':
            case 'u':
                ++count;
                break;
        }
    }

    return count;
}
