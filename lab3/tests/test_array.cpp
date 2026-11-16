#include "figure_array.hpp"
#include "hexagon.hpp"
#include "octagon.hpp"
#include "pentagon.hpp"
#include "regular_polygon.hpp"

#include <gtest/gtest.h>

#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace lab3::test {
namespace {

Pentagon* makePentagon(double centerX = 0.0, double centerY = 0.0, double radius = 1.0) {
    return new Pentagon(regularPolygon<5>(centerX, centerY, radius));
}

Hexagon* makeHexagon() {
    return new Hexagon(regularPolygon<6>());
}

Octagon* makeOctagon() {
    return new Octagon(regularPolygon<8>());
}

}  // namespace

TEST(FigureArrayTest, AddsFiguresAndCalculatesTotalArea) {
    FigureArray figures;
    EXPECT_TRUE(figures.empty());
    figures.add(makePentagon());
    figures.add(makeHexagon());
    figures.add(makeOctagon());

    ASSERT_EQ(figures.size(), 3);
    const double expected = regularPolygonArea<5>() + regularPolygonArea<6>()
        + regularPolygonArea<8>();
    EXPECT_NEAR(figures.totalArea(), expected, 1e-12);
    EXPECT_STREQ(figures.at(1)->typeName(), "Hexagon");
}

TEST(FigureArrayTest, GrowsAutomaticallyAndRemovesByIndex) {
    FigureArray figures(1);
    for (int i = 0; i < 10; ++i) {
        figures.add(makePentagon(static_cast<double>(i)));
    }
    ASSERT_EQ(figures.size(), 10);
    EXPECT_GE(figures.capacity(), 10);

    figures.remove(0);
    figures.remove(7);
    ASSERT_EQ(figures.size(), 8);
    EXPECT_NEAR(figures.at(0)->center().x, 1.0, 1e-12);
    EXPECT_NEAR(figures.at(7)->center().x, 9.0, 1e-12);
}

TEST(FigureArrayTest, RejectsInvalidIndicesAndNullPointer) {
    FigureArray figures;
    EXPECT_THROW(figures.remove(0), std::out_of_range);
    EXPECT_THROW(figures.at(0), std::out_of_range);
    EXPECT_THROW(figures.add(nullptr), std::invalid_argument);
    figures.add(makePentagon());
    const FigureArray& constant = figures;
    EXPECT_THROW(figures.remove(1), std::out_of_range);
    EXPECT_THROW(constant.at(1), std::out_of_range);
    EXPECT_EQ(figures.size(), 1);
}

TEST(FigureArrayTest, CopyConstructorPerformsDeepCopy) {
    FigureArray original;
    original.add(makePentagon());
    original.add(makeHexagon());
    FigureArray copy(original);

    ASSERT_EQ(copy.size(), original.size());
    for (std::size_t i = 0; i < original.size(); ++i) {
        EXPECT_NE(copy.at(i), original.at(i));
        EXPECT_TRUE(*copy.at(i) == *original.at(i));
    }

    copy.remove(0);
    ASSERT_EQ(original.size(), 2);
    EXPECT_STREQ(original.at(0)->typeName(), "Pentagon");
}

TEST(FigureArrayTest, CopyIsIndependentFromOriginal) {
    FigureArray original;
    original.add(makePentagon());
    FigureArray copy(original);

    std::istringstream input;
    const auto movedVertices = regularPolygon<5>(10.0, 20.0, 2.0);
    std::ostringstream coordinates;
    coordinates << std::setprecision(17);
    for (const Point& point : movedVertices) {
        coordinates << point.x << ' ' << point.y << ' ';
    }
    input.str(coordinates.str());
    input >> *copy.at(0);

    EXPECT_NEAR(original.at(0)->center().x, 0.0, 1e-12);
    EXPECT_NEAR(copy.at(0)->center().x, 10.0, 1e-12);
    EXPECT_NEAR(original.at(0)->center().y, 0.0, 1e-12);
    EXPECT_NEAR(copy.at(0)->center().y, 20.0, 1e-12);
}

TEST(FigureArrayTest, CopyAssignmentReplacesContentsDeeply) {
    FigureArray original;
    original.add(makePentagon());
    original.add(makeHexagon());
    FigureArray copy;
    copy.add(makeOctagon());
    copy = original;

    ASSERT_EQ(copy.size(), 2);
    EXPECT_NE(copy.at(0), original.at(0));
    EXPECT_TRUE(*copy.at(1) == *original.at(1));
    copy = copy;
    EXPECT_EQ(copy.size(), 2);
}

TEST(FigureArrayTest, MoveConstructorAndAssignmentTransferOwnership) {
    FigureArray original;
    original.add(makePentagon());
    original.add(makeHexagon());
    const double area = original.totalArea();

    FigureArray moved(std::move(original));
    EXPECT_EQ(original.size(), 0);
    EXPECT_EQ(original.capacity(), 0);
    ASSERT_EQ(moved.size(), 2);
    EXPECT_NEAR(moved.totalArea(), area, 1e-12);

    FigureArray assigned;
    assigned.add(makeOctagon());
    assigned = std::move(moved);
    EXPECT_EQ(moved.size(), 0);
    ASSERT_EQ(assigned.size(), 2);
    EXPECT_NEAR(assigned.totalArea(), area, 1e-12);
    assigned = std::move(assigned);
    EXPECT_EQ(assigned.size(), 2);
}

TEST(FigureArrayTest, MovedFromArrayCanBeReused) {
    FigureArray original;
    original.add(makePentagon());
    FigureArray moved(std::move(original));
    original.add(makeHexagon());
    EXPECT_EQ(original.size(), 1);
    EXPECT_EQ(moved.size(), 1);
    EXPECT_STREQ(original.at(0)->typeName(), "Hexagon");
}

}  // namespace lab3::test
