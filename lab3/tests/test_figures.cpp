#include "figure.hpp"
#include "hexagon.hpp"
#include "octagon.hpp"
#include "pentagon.hpp"
#include "regular_polygon.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace lab3::test {
namespace {

template <typename T>
void verifyCopyAndMove() {
    T original(regularPolygon<T::vertexCount>(2.0, -3.0, 2.0));
    T copy(original);
    EXPECT_EQ(copy, original);

    T copyAssigned;
    copyAssigned = original;
    EXPECT_EQ(copyAssigned, original);

    T moved(std::move(copy));
    EXPECT_EQ(moved, original);

    T moveAssigned;
    moveAssigned = std::move(moved);
    EXPECT_EQ(moveAssigned, original);
}

std::string coordinates(const Pentagon::Vertices& vertices) {
    std::ostringstream input;
    input << std::setprecision(17);
    for (const Point& point : vertices) {
        input << point.x << ' ' << point.y << ' ';
    }
    return input.str();
}

}  // namespace

TEST(PentagonTest, CalculatesCenterAndArea) {
    const Pentagon figure(regularPolygon<5>());
    const Point center = figure.center();
    EXPECT_NEAR(center.x, 0.0, 1e-12);
    EXPECT_NEAR(center.y, 0.0, 1e-12);
    EXPECT_NEAR(static_cast<double>(figure), regularPolygonArea<5>(), 1e-12);
}

TEST(HexagonTest, CalculatesCenterAndAreaAfterTranslation) {
    const Hexagon figure(regularPolygon<6>(4.0, -2.0, 3.0));
    const Point center = figure.center();
    EXPECT_NEAR(center.x, 4.0, 1e-12);
    EXPECT_NEAR(center.y, -2.0, 1e-12);
    EXPECT_NEAR(static_cast<double>(figure), regularPolygonArea<6>(3.0), 1e-10);
}

TEST(OctagonTest, CalculatesCenterAndAreaAfterTranslation) {
    const Octagon figure(regularPolygon<8>(-5.0, 7.0, 2.5, 0.2));
    const Point center = figure.center();
    EXPECT_NEAR(center.x, -5.0, 1e-12);
    EXPECT_NEAR(center.y, 7.0, 1e-12);
    EXPECT_NEAR(static_cast<double>(figure), regularPolygonArea<8>(2.5), 1e-10);
}

TEST(FigureTest, StreamInputAndOutputUseVirtualInterface) {
    Pentagon figure;
    const auto expected = regularPolygon<5>(3.0, 2.0, 2.0, 0.1);
    std::istringstream input(coordinates(expected));
    Figure* base = &figure;
    input >> *base;
    ASSERT_FALSE(input.fail());
    EXPECT_NEAR(figure.center().x, 3.0, 1e-12);
    EXPECT_NEAR(figure.center().y, 2.0, 1e-12);

    std::ostringstream output;
    output << *base;
    EXPECT_FALSE(output.str().empty());
    EXPECT_NE(output.str().find('('), std::string::npos);
}

TEST(FigureTest, FailedCoordinateInputDoesNotChangeFigure) {
    Pentagon original(regularPolygon<5>());
    Pentagon figure(original);
    std::istringstream input("1 0 bad 1 0 0 0 0 0 0");
    input >> figure;
    EXPECT_TRUE(input.fail());
    EXPECT_EQ(figure, original);
}

TEST(FigureTest, InvalidGeometryDoesNotChangeFigure) {
    Pentagon original(regularPolygon<5>());
    Pentagon figure(original);
    std::istringstream input("0 0 1 0 2 0 3 0 4 0");
    EXPECT_THROW(input >> figure, std::invalid_argument);
    EXPECT_EQ(figure, original);
}

TEST(FigureTest, ConversionToDoubleReturnsArea) {
    const Pentagon pentagon(regularPolygon<5>());
    const Hexagon hexagon(regularPolygon<6>());
    const Octagon octagon(regularPolygon<8>());
    const Figure& base = octagon;
    EXPECT_DOUBLE_EQ(static_cast<double>(base), static_cast<double>(octagon));
    EXPECT_NEAR(static_cast<double>(pentagon), regularPolygonArea<5>(), 1e-12);
    EXPECT_NEAR(static_cast<double>(hexagon), regularPolygonArea<6>(), 1e-12);
}

TEST(FigureTest, CloneAndComparisonArePolymorphic) {
    const Pentagon pentagon(regularPolygon<5>());
    const Hexagon hexagon(regularPolygon<6>());
    const Figure& base = pentagon;
    std::unique_ptr<Figure> copy(base.clone());
    ASSERT_NE(copy.get(), &pentagon);
    EXPECT_TRUE(base == *copy);
    EXPECT_FALSE(base != *copy);
    EXPECT_FALSE(base == hexagon);
    EXPECT_STREQ(copy->typeName(), "Pentagon");
}

TEST(FigureTest, EqualityUsesCoordinateTolerance) {
    auto vertices = regularPolygon<5>();
    auto almostSame = vertices;
    almostSame[2].x += 1e-11;
    almostSame[4].y -= 1e-11;
    const Pentagon lhs(vertices);
    const Pentagon rhs(almostSame);
    const Pentagon different(regularPolygon<5>(0.0, 0.0, 1.1));
    EXPECT_EQ(lhs, rhs);
    EXPECT_NE(lhs, different);
}

TEST(FigureTest, SupportsCopyAndMoveForEveryFigure) {
    verifyCopyAndMove<Pentagon>();
    verifyCopyAndMove<Hexagon>();
    verifyCopyAndMove<Octagon>();
}

TEST(FigureValidationTest, RejectsDegenerateAndNonConvexFigures) {
    Pentagon::Vertices degenerate{};
    EXPECT_THROW((void)Pentagon(degenerate), std::invalid_argument);

    auto nonConvex = regularPolygon<5>();
    nonConvex[2] = {0.2, 0.2};
    EXPECT_THROW((void)Pentagon(nonConvex), std::invalid_argument);
}

TEST(FigureValidationTest, RejectsIrregularAndNonFiniteFigures) {
    auto irregular = regularPolygon<6>();
    irregular[1].x += 0.3;
    EXPECT_THROW((void)Hexagon(irregular), std::invalid_argument);

    auto nonFinite = regularPolygon<8>();
    nonFinite[0].x = std::numeric_limits<double>::infinity();
    EXPECT_THROW((void)Octagon(nonFinite), std::invalid_argument);
}

TEST(FigureValidationTest, AcceptsClockwiseVertexOrder) {
    auto vertices = regularPolygon<8>();
    std::reverse(vertices.begin(), vertices.end());
    EXPECT_NO_THROW(Octagon(vertices));
}

}  // namespace lab3::test
