#include <gtest/gtest.h>
#include "intersections.hpp"
#include "geometry.hpp"
#include <stdexcept>

using namespace geometry;

// Тест поддерживаемых комбинаций пересечений геометрических примитивов
TEST(IntersectionTest, ValidCombinations) {
    // 1. Тест: Line & Line
    Line l1{Point2D{0.0, 0.0}, Point2D{2.0, 2.0}};
    Line l2{Point2D{0.0, 2.0}, Point2D{2.0, 0.0}};
    
    auto pt1 = intersections::GetIntersectPoint(l1, l2);
    ASSERT_TRUE(pt1.has_value());
    EXPECT_DOUBLE_EQ(pt1->x, 1.0);
    EXPECT_DOUBLE_EQ(pt1->y, 1.0);

    // 2. Тест: Line & Circle
    Circle c{Point2D{0.0, 0.0}, 2.0};
    Line l3{Point2D{-5.0, 0.0}, Point2D{5.0, 0.0}};
    
    auto pt2 = intersections::GetIntersectPoint(l3, c);
    ASSERT_TRUE(pt2.has_value());
    EXPECT_DOUBLE_EQ(pt2->x, -2.0); // Ближайшая точка пересечения по лучу
    EXPECT_DOUBLE_EQ(pt2->y, 0.0);

    // 3. Тест: Circle & Circle
    Circle c1{Point2D{0.0, 0.0}, 2.0};
    Circle c2{Point2D{3.0, 0.0}, 2.0};
    
    auto pt3 = intersections::GetIntersectPoint(c1, c2);
    ASSERT_TRUE(pt3.has_value()); // Пересекаются на X = 1.5
    EXPECT_DOUBLE_EQ(pt3->x, 1.5);
}

// Тест проверки жесткого требования ТЗ: неподдерживаемые типы обязаны бросать исключение
TEST(IntersectionTest, UnsupportedCombinationsThrow) {
    Triangle t{Point2D{0,0}, Point2D{1,0}, Point2D{0,1}};
    Rectangle r{Point2D{0,0}, 2.0, 2.0};
    Circle c{Point2D{0,0}, 5.0};

    // Проверяем, что комбинация Triangle & Rectangle генерирует std::logic_error
    EXPECT_THROW(intersections::GetIntersectPoint(t, r), std::logic_error);
    
    // Проверяем, что комбинация Triangle & Circle также генерирует ошибку
    EXPECT_THROW(intersections::GetIntersectPoint(t, c), std::logic_error);
}
