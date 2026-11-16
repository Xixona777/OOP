#include "TestFigures.hpp"

#include "Array.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>

namespace {
struct MoveProbe {
    static inline int copyConstructions = 0;
    static inline int copyAssignments = 0;
    static inline int moveConstructions = 0;
    static inline int moveAssignments = 0;

    int value{};

    MoveProbe() = default;
    explicit MoveProbe(int value) : value(value) {}
    MoveProbe(const MoveProbe& other) : value(other.value) {
        ++copyConstructions;
    }
    MoveProbe(MoveProbe&& other) noexcept : value(other.value) {
        ++moveConstructions;
    }
    MoveProbe& operator=(const MoveProbe& other) {
        value = other.value;
        ++copyAssignments;
        return *this;
    }
    MoveProbe& operator=(MoveProbe&& other) noexcept {
        value = other.value;
        ++moveAssignments;
        return *this;
    }

    static void resetCounters() {
        copyConstructions = 0;
        copyAssignments = 0;
        moveConstructions = 0;
        moveAssignments = 0;
    }
};
}

TEST(Array, StartsEmptyAndAddsElements) {
    Array<int> values;
    EXPECT_EQ(values.size(), 0);
    EXPECT_EQ(values.capacity(), 0);
    values.push_back(10);
    values.push_back(20);
    values.push_back(30);
    ASSERT_EQ(values.size(), 3);
    EXPECT_EQ(values.capacity(), 4);
    EXPECT_EQ(values[0], 10);
    EXPECT_EQ(values[1], 20);
    EXPECT_EQ(values[2], 30);
}

TEST(Array, RemovesByIndexAndShiftsElements) {
    Array<int> values;
    for (int value = 0; value < 5; ++value) {
        values.push_back(value);
    }
    values.remove(1);
    ASSERT_EQ(values.size(), 4);
    EXPECT_EQ(values[0], 0);
    EXPECT_EQ(values[1], 2);
    EXPECT_EQ(values[2], 3);
    EXPECT_EQ(values[3], 4);
}

TEST(Array, ChecksBoundsForAccessAndRemoval) {
    Array<int> values;
    EXPECT_THROW(values[0], std::out_of_range);
    EXPECT_THROW(values.remove(0), std::out_of_range);
    values.push_back(1);
    EXPECT_THROW(values[1], std::out_of_range);
    EXPECT_THROW(values.remove(1), std::out_of_range);
    const auto& constant = values;
    EXPECT_THROW(constant[2], std::out_of_range);
}

TEST(Array, GrowsAutomaticallyByMovingExistingElements) {
    Array<MoveProbe> values;
    values.push_back(MoveProbe(1));
    MoveProbe::resetCounters();

    values.push_back(MoveProbe(2));

    EXPECT_EQ(values.capacity(), 2);
    EXPECT_EQ(MoveProbe::copyConstructions, 0);
    EXPECT_EQ(MoveProbe::copyAssignments, 0);
    EXPECT_EQ(MoveProbe::moveAssignments, 2);
    EXPECT_EQ(values[0].value, 1);
    EXPECT_EQ(values[1].value, 2);
}

TEST(Array, CopyConstructorAndAssignmentAreIndependent) {
    Array<int> original;
    original.push_back(1);
    original.push_back(2);
    Array<int> copied(original);
    copied[0] = 9;
    EXPECT_EQ(original[0], 1);

    Array<int> assigned;
    assigned = original;
    assigned[1] = 8;
    EXPECT_EQ(original[1], 2);
    EXPECT_EQ(assigned.capacity(), original.capacity());
}

TEST(Array, MoveConstructorAndAssignmentTransferStorage) {
    Array<int> original;
    original.push_back(4);
    original.push_back(5);
    Array<int> moved(std::move(original));
    EXPECT_EQ(original.size(), 0);
    EXPECT_EQ(original.capacity(), 0);
    ASSERT_EQ(moved.size(), 2);
    EXPECT_EQ(moved[0], 4);

    Array<int> assigned;
    assigned = std::move(moved);
    EXPECT_EQ(moved.size(), 0);
    EXPECT_EQ(moved.capacity(), 0);
    EXPECT_EQ(assigned[1], 5);
}

TEST(Array, SupportsSharedPointersToBaseFigures) {
    Array<std::shared_ptr<Figure<int>>> figures;
    figures.push_back(std::make_shared<Hexagon<int>>());
    std::shared_ptr<Figure<int>> clone = figures[0]->clone();
    figures.push_back(std::move(clone));
    ASSERT_EQ(figures.size(), 2);
    EXPECT_EQ(figures[0]->type(), "Hexagon");
    EXPECT_EQ(*figures[0], *figures[1]);
    EXPECT_DOUBLE_EQ(static_cast<double>(*figures[0]), 0.0);
    figures.remove(0);
    EXPECT_EQ(figures.size(), 1);
}

TEST(Array, SupportsConcreteHexagonObjects) {
    Array<Hexagon<int>> figures;
    Hexagon<int> original;
    figures.push_back(original);
    figures.push_back(Hexagon<int>{});
    ASSERT_EQ(figures.size(), 2);
    EXPECT_EQ(figures[0], figures[1]);

    Array<Hexagon<int>> copied(figures);
    copied[0].vertex(0).x = 7;
    EXPECT_EQ(figures[0].vertex(0).x, 0);
    EXPECT_NE(copied[0], figures[0]);
}
