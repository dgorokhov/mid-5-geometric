#include <gtest/gtest.h>
#include "convex_hull.hpp"
#include "geometry.hpp"
#include <vector>

using namespace geometry;

// Тест успешного построения оболочки вокруг набора точек
TEST(ConvexHullTest, GrahamScanSuccess) {
    std::vector<Point2D> points = {
        {0.0, 0.0}, {4.0, 0.0}, {4.0, 4.0}, {0.0, 4.0},
        {2.0, 2.0}, {1.0, 1.0}, {3.0, 3.0} // Внутренние точки
    };

    auto result = convex_hull::GrahamScan(points);
    
    // Проверяем, что std::expected содержит успешный результат
    ASSERT_TRUE(result.has_value());
    
    std::vector<Point2D> hull = result.value();
    // Оболочкой квадрата должны стать его 4 вершины
    EXPECT_EQ(hull.size(), 4);
}

// Тест обработки ошибки: слишком мало точек
TEST(ConvexHullTest, TooFewPointsError) {
    std::vector<Point2D> points = {{0.0, 0.0}, {1.0, 1.0}};

    auto result = convex_hull::GrahamScan(points);
    
    // Должен вернуться std::unexpected со значением TooFewPoints
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), convex_hull::ConvexHullError::TooFewPoints);
}

// Тест обработки ошибки: выроженный вар
TEST(ConvexHullTest, DegenerateGeom) {
    std::vector<Point2D> points = {{0.0, 0.0}, {1.0, 1.0}};

    auto result = convex_hull::GrahamScan(points);
    
    // Должен вернуться std::unexpected со значением TooFewPoints
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), convex_hull::ConvexHullError::TooFewPoints);
}