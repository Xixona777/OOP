#include <gtest/gtest.h>

#include "vowels.hpp"

TEST(VowelsTest, EmptyString) {
    EXPECT_EQ(countVowels(""), 0);
}

TEST(VowelsTest, StringWithoutVowels) {
    EXPECT_EQ(countVowels("bcd fgh jklm npqrst vwxyz"), 0);
}

TEST(VowelsTest, StringWithOnlyVowels) {
    EXPECT_EQ(countVowels("aeiouaeiou"), 10);
}

TEST(VowelsTest, StringWithSpaces) {
    EXPECT_EQ(countVowels("this is a test"), 4);
}

TEST(VowelsTest, RegularText) {
    EXPECT_EQ(countVowels("hello world"), 3);
}
