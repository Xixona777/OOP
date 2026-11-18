#include "dice.h"

namespace lab7 {

RandomDice::RandomDice(std::uint32_t seed) : generator_(seed) {}

int RandomDice::roll() {
    return distribution_(generator_);
}

} // namespace lab7
