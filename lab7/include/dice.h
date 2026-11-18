#pragma once

#include <cstdint>
#include <random>

namespace lab7 {

class IDice {
public:
    virtual ~IDice() = default;
    [[nodiscard]] virtual int roll() = 0;
};

class RandomDice final : public IDice {
public:
    explicit RandomDice(std::uint32_t seed);

    [[nodiscard]] int roll() override;

private:
    std::mt19937 generator_;
    std::uniform_int_distribution<int> distribution_{1, 6};
};

} // namespace lab7
