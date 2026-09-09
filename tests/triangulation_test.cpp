#include <gtest/gtest.h>
#include "triangulation.hpp"
#include "geometry.hpp"
#include <vector>

using namespace geometry;

TEST(TriangulationTest, DelaunaySuccess) {
    // 5 точек, == выпуклый пятиугольник с точкой внутри
    std::vector<Point2D> points = {
        {0.0, 0.0}, {10.0, 0.0}, {5.0, 8.0}, {15.0, 5.0}, {2.0, 12.0}
    };
   auto result = triangulation::DelaunayTriangulation(points);
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(result->size(), 0); // Проверяем, что сетка треугольников сгенерирована
}

TEST(TriangulationTest, TooFewPointsError) {
    std::vector<Point2D> points = {{0.0, 0.0}, {1.0, 1.0}};
   auto result = triangulation::DelaunayTriangulation(points);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), triangulation::TriangulationError::TooFewPoints);
}
