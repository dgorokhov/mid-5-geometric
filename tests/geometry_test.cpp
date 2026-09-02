#include <gtest/gtest.h>
#include "geometry.hpp"
#include <cmath>

using namespace geometry;

// Тестирование базовых математических операций над Point2D
TEST(GeometryTest, Point2DMathOperators) {
    Point2D p1{3.0, 4.0};
    Point2D p2{1.0, 2.0};

    // Сложение и вычитание структур
    EXPECT_EQ(p1 + p2, Point2D(4.0, 6.0));
    EXPECT_EQ(p1 - p2, Point2D(2.0, 2.0));

    // Умножение и деление на double
    EXPECT_EQ(p1 * 2.0, Point2D(6.0, 8.0));
    EXPECT_EQ(p1 / 2.0, Point2D(1.5, 2.0));
}

// Тестирование геометрических функций точек
TEST(GeometryTest, Point2DGeometryOperations) {
    Point2D p1{3.0, 4.0};
    Point2D p2{1.0, 2.0};

    // Длина, расстояние и нормализация
    EXPECT_DOUBLE_EQ(p1.Length(), 5.0);
    EXPECT_DOUBLE_EQ(p1.DistanceTo(p2), std::sqrt(4.0 + 4.0));
    
    Point2D p1_norm = p1.Normalize();
    EXPECT_DOUBLE_EQ(p1_norm.Length(), 1.0);

    // Скалярное (Dot) и векторное (Cross) произведения
    EXPECT_DOUBLE_EQ(p1.Dot(p2), 11.0);
    EXPECT_DOUBLE_EQ(p1.Cross(p2), 2.0);
}

// Тестирование генерации и пересечения Axis-Aligned Bounding Box (AABB)
TEST(GeometryTest, BoundingBoxOverlaps) {
    Line l{Point2D{0.0, 5.0}, Point2D{4.0, 1.0}};
    BoundingBox bbox = l.BoundBox();

    // Проверка корректности крайних координат прямоугольника
    EXPECT_DOUBLE_EQ(bbox.min_x, 0.0);
    EXPECT_DOUBLE_EQ(bbox.min_y, 1.0);
    EXPECT_DOUBLE_EQ(bbox.max_x, 4.0);
    EXPECT_DOUBLE_EQ(bbox.max_y, 5.0);
    
    // Проверка пересечения двух BoundingBox (Overlaps)
    BoundingBox intersecting_box{3.0, 0.0, 5.0, 2.0};
    BoundingBox distant_box{10.0, 10.0, 12.0, 12.0};

    EXPECT_TRUE(bbox.Overlaps(intersecting_box));
    EXPECT_FALSE(bbox.Overlaps(distant_box));
}
