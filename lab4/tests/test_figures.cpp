#include "TestFigures.hpp"

#include "Array.hpp"
#include "Figure.hpp"
#include "Scalar.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

static_assert(Scalar<int>);
static_assert(Scalar<double>);
static_assert(!Scalar<std::string>);
static_assert(!Scalar<Point<int>>);
static_assert(std::is_same_v<decltype(std::declval<const Hexagon<int>&>().center()),
                             Point<double>>);

TEST(ScalarAndPoint, SupportsIntAndDouble) {
    Point<int> integer{3, -2};
    Point<double> real{1.25, -4.5};
    EXPECT_EQ(integer.x, 3);
    EXPECT_EQ(integer.y, -2);
    EXPECT_DOUBLE_EQ(real.x, 1.25);
    EXPECT_DOUBLE_EQ(real.y, -4.5);
    EXPECT_EQ(integer, (Point<int>{3, -2}));
    EXPECT_NE(real, (Point<double>{0.0, -4.5}));
}

TEST(Triangle, CalculatesAreaAndCenter) {
    Triangle<double> triangle(trianglePoints());
    EXPECT_DOUBLE_EQ(static_cast<double>(triangle), std::sqrt(3.0));
    const auto center = triangle.center();
    EXPECT_NEAR(center.x, 1.0, 1e-12);
    EXPECT_NEAR(center.y, std::sqrt(3.0) / 3.0, 1e-12);
    EXPECT_EQ(triangle.type(), "Triangle");
}

TEST(Hexagon, CalculatesAreaAndCenter) {
    Hexagon<double> hexagon(hexagonPoints());
    EXPECT_NEAR(static_cast<double>(hexagon), 6.0 * std::sqrt(3.0), 1e-12);
    const auto center = hexagon.center();
    EXPECT_NEAR(center.x, 0.0, 1e-12);
    EXPECT_NEAR(center.y, 0.0, 1e-12);
    EXPECT_EQ(hexagon.type(), "Hexagon");
}

TEST(Octagon, CalculatesAreaAndCenter) {
    Octagon<double> octagon(octagonPoints());
    EXPECT_NEAR(static_cast<double>(octagon), 2.0 * std::sqrt(2.0), 1e-12);
    const auto center = octagon.center();
    EXPECT_NEAR(center.x, 0.0, 1e-12);
    EXPECT_NEAR(center.y, 0.0, 1e-12);
    EXPECT_EQ(octagon.type(), "Octagon");
}

TEST(FigureStreams, WritesVertices) {
    Triangle<double> triangle(trianglePoints());
    std::ostringstream output;
    output << triangle;
    EXPECT_EQ(output.str(), "(0, 0) (2, 0) (1, 1.73205)");
}

TEST(FigureStreams, ReadsValidVertices) {
    Triangle<double> triangle;
    std::istringstream input("0 0 2 0 1 1.7320508075688772");
    input >> triangle;
    EXPECT_TRUE(input);
    EXPECT_DOUBLE_EQ(static_cast<double>(triangle), std::sqrt(3.0));
}

TEST(FigureStreams, RejectsMalformedInput) {
    Triangle<double> triangle;
    std::istringstream input("0 0 two 0 1 1.732");
    input >> triangle;
    EXPECT_TRUE(input.fail());
}

TEST(FigureStreams, RejectsIrregularInputAndPreservesObject) {
    Triangle<double> triangle(trianglePoints());
    const Triangle<double> original(triangle);
    std::istringstream input("0 0 1 0 2 0");
    input >> triangle;
    EXPECT_TRUE(input.fail());
    EXPECT_EQ(triangle, original);
}

TEST(FigureValidation, RejectsDegeneratePolygon) {
    const std::array<Point<double>, 3> points{{{0.0, 0.0}, {1.0, 0.0}, {2.0, 0.0}}};
    EXPECT_THROW(Triangle<double> triangle(points), std::invalid_argument);
}

TEST(FigureValidation, RejectsNonConvexPolygon) {
    const std::array<Point<double>, 6> points{{{2.0, 0.0}, {0.2, 0.2}, {-1.0, 1.7},
                                               {-2.0, 0.0}, {-1.0, -1.7}, {1.0, -1.7}}};
    EXPECT_THROW(Hexagon<double> hexagon(points), std::invalid_argument);
}

TEST(FigureValidation, RejectsNonRegularPolygon) {
    const std::array<Point<double>, 3> points{{{0.0, 0.0}, {3.0, 0.0}, {1.0, 1.0}}};
    EXPECT_THROW(Triangle<double> triangle(points), std::invalid_argument);
}

TEST(FigureCopy, CopyConstructorMakesIndependentPoints) {
    Hexagon<double> original(hexagonPoints());
    Hexagon<double> copy(original);
    ASSERT_EQ(copy, original);
    copy.vertex(0).x = 100.0;
    EXPECT_NE(copy, original);
    EXPECT_DOUBLE_EQ(original.vertex(0).x, 2.0);
}

TEST(FigureCopy, CopyAssignmentMakesIndependentPoints) {
    Octagon<double> original(octagonPoints());
    Octagon<double> copy;
    copy = original;
    ASSERT_EQ(copy, original);
    copy.vertex(1).y = 50.0;
    EXPECT_NE(copy, original);
    EXPECT_NEAR(original.vertex(1).y, std::sqrt(0.5), 1e-12);
}

TEST(FigureMove, MoveConstructorTransfersVertices) {
    Triangle<double> original(trianglePoints());
    Triangle<double> moved(std::move(original));
    EXPECT_DOUBLE_EQ(static_cast<double>(moved), std::sqrt(3.0));
    EXPECT_EQ(moved.vertex(0), (Point<double>{0.0, 0.0}));
}

TEST(FigureMove, MoveAssignmentTransfersVertices) {
    Hexagon<double> original(hexagonPoints());
    Hexagon<double> moved;
    moved = std::move(original);
    EXPECT_NEAR(static_cast<double>(moved), 6.0 * std::sqrt(3.0), 1e-12);
}

TEST(FigureComparison, ComparesCoordinatesAndDynamicTypes) {
    Triangle<double> triangle(trianglePoints());
    Triangle<double> same(trianglePoints());
    Triangle<double> different(trianglePoints());
    different.vertex(0).x = -1.0;
    Hexagon<double> hexagon(hexagonPoints());
    EXPECT_EQ(triangle, same);
    EXPECT_NE(triangle, different);
    EXPECT_NE(triangle, hexagon);
}

TEST(FigureClone, ClonesThroughBaseInterface) {
    const Octagon<double> original(octagonPoints());
    const Figure<double>& base = original;
    auto copy = base.clone();
    ASSERT_NE(copy, nullptr);
    EXPECT_EQ(copy->type(), "Octagon");
    EXPECT_EQ(*copy, original);
}

TEST(FigurePolymorphism, CalculatesTotalAreaThroughBase) {
    Array<std::shared_ptr<Figure<double>>> figures;
    figures.push_back(std::make_shared<Triangle<double>>(trianglePoints()));
    figures.push_back(std::make_shared<Hexagon<double>>(hexagonPoints()));
    figures.push_back(std::make_shared<Octagon<double>>(octagonPoints()));

    double total = 0.0;
    for (std::size_t i = 0; i < figures.size(); ++i) {
        total += static_cast<double>(*figures[i]);
        EXPECT_GE(figures[i]->center().x, -10.0);
    }
    EXPECT_NEAR(total, 7.0 * std::sqrt(3.0) + 2.0 * std::sqrt(2.0), 1e-12);
}

TEST(FigureInt, CenterUsesFloatingPointCoordinates) {
    Hexagon<int> hexagon;
    hexagon.vertex(0).x = 1;
    const auto center = hexagon.center();
    EXPECT_NEAR(center.x, 1.0 / 6.0, 1e-12);
    EXPECT_DOUBLE_EQ(center.y, 0.0);
}
