#include "Three.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <type_traits>
#include <utility>

static_assert(std::is_nothrow_move_constructible<Three>::value,
              "Three move constructor must be noexcept");
static_assert(std::has_virtual_destructor<Three>::value,
              "Three destructor must be virtual");
static_assert(std::is_nothrow_destructible<Three>::value,
              "Three destructor must be noexcept");

TEST(ThreeConstructors, DefaultCreatesZero) {
    Three value;
    EXPECT_EQ(value.toString(), "0");
}

TEST(ThreeConstructors, FillCreatesRepeatedDigits) {
    Three value(4, 2);
    EXPECT_EQ(value.toString(), "2222");
}

TEST(ThreeConstructors, InitializerListUsesConventionalDigitOrder) {
    Three value{1, 0, 2};
    EXPECT_EQ(value.toString(), "102");
}

TEST(ThreeConstructors, EmptyInitializerListCreatesZero) {
    Three value(std::initializer_list<unsigned char>{});
    EXPECT_EQ(value.toString(), "0");
}

TEST(ThreeConstructors, StringCreatesNumber) {
    Three value("1201");
    EXPECT_EQ(value.toString(), "1201");
}

TEST(ThreeConstructors, InvalidSizeThrows) {
    EXPECT_THROW(Three(0, 0), std::invalid_argument);
}

TEST(ThreeConstructors, InvalidFillDigitThrows) {
    EXPECT_THROW(Three(3, 3), std::invalid_argument);
}

TEST(ThreeConstructors, InvalidInitializerListDigitThrows) {
    EXPECT_THROW(Three({1, 3, 0}), std::invalid_argument);
}

TEST(ThreeConstructors, InvalidStringThrows) {
    EXPECT_THROW(Three(""), std::invalid_argument);
    EXPECT_THROW(Three("103"), std::invalid_argument);
    EXPECT_THROW(Three("12a"), std::invalid_argument);
    EXPECT_THROW(Three("-1"), std::invalid_argument);
}

TEST(ThreeCopy, CopyConstructorPerformsDeepCopy) {
    Three original("102");
    Three copied(original);
    Three moved(std::move(original));

    EXPECT_EQ(copied.toString(), "102");
    EXPECT_EQ(moved.toString(), "102");
    EXPECT_TRUE(copied.equals(moved));
}

TEST(ThreeCopy, CopyMethodReturnsIndependentObject) {
    Three original("210");
    Three copied = original.copy();
    Three moved(std::move(original));

    EXPECT_EQ(copied.toString(), "210");
    EXPECT_EQ(moved.toString(), "210");
    EXPECT_TRUE(copied.equals(moved));
}

TEST(ThreeMove, MoveConstructorTransfersValueAndLeavesValidSource) {
    Three original("1201");
    Three moved(std::move(original));

    EXPECT_EQ(moved.toString(), "1201");
    EXPECT_EQ(original.toString(), "0");
    EXPECT_TRUE(original.equals(Three()));
    EXPECT_EQ(original.copy().toString(), "0");
    EXPECT_EQ(original.add(Three("1")).toString(), "1");
    EXPECT_EQ(original.subtract(Three()).toString(), "0");
}

TEST(ThreeAddition, AddsWithCarry) {
    const Three left("222");
    const Three right("1");

    EXPECT_EQ(left.add(right).toString(), "1000");
    EXPECT_EQ(left.toString(), "222");
    EXPECT_EQ(right.toString(), "1");
}

TEST(ThreeAddition, AddsDifferentLengths) {
    EXPECT_EQ(Three("120").add(Three("12")).toString(), "202");
    EXPECT_EQ(Three("2").add(Three("2")).toString(), "11");
    EXPECT_EQ(Three().add(Three("102")).toString(), "102");
}

TEST(ThreeSubtraction, SubtractsWithBorrow) {
    const Three left("1000");
    const Three right("1");

    EXPECT_EQ(left.subtract(right).toString(), "222");
    EXPECT_EQ(left.toString(), "1000");
    EXPECT_EQ(right.toString(), "1");
}

TEST(ThreeSubtraction, SubtractsDifferentLengths) {
    EXPECT_EQ(Three("120").subtract(Three("12")).toString(), "101");
    EXPECT_EQ(Three("201").subtract(Three("2")).toString(), "122");
}

TEST(ThreeSubtraction, EqualNumbersProduceZero) {
    EXPECT_EQ(Three("120").subtract(Three("00120")).toString(), "0");
}

TEST(ThreeSubtraction, NegativeResultThrows) {
    EXPECT_THROW(Three("1").subtract(Three("2")), std::underflow_error);
}

TEST(ThreeComparison, ComparesNumbersWithDifferentLengths) {
    const Three shorter("222");
    const Three longer("1000");

    EXPECT_TRUE(longer.greater(shorter));
    EXPECT_TRUE(shorter.less(longer));
    EXPECT_FALSE(shorter.greater(longer));
    EXPECT_FALSE(longer.less(shorter));
    EXPECT_FALSE(shorter.equals(longer));
}

TEST(ThreeComparison, ComparesNumbersWithEqualLengths) {
    const Three left("121");
    const Three right("120");
    const Three same("0121");

    EXPECT_TRUE(left.greater(right));
    EXPECT_TRUE(right.less(left));
    EXPECT_TRUE(left.equals(same));
    EXPECT_FALSE(left.greater(same));
    EXPECT_FALSE(left.less(same));
}

TEST(ThreeLeadingZeros, RemovesLeadingZerosFromAllConstructors) {
    EXPECT_EQ(Three(5, 0).toString(), "0");
    EXPECT_EQ(Three({0, 0, 1, 2}).toString(), "12");
    EXPECT_EQ(Three("000102").toString(), "102");
    EXPECT_EQ(Three("000").toString(), "0");
}
