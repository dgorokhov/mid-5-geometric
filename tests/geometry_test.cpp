#include <gtest/gtest.h>
#include "geometry.hpp"
#include <cmath>

using namespace geometry;

// базовые математические опер-ий
TEST(GeometryTest, Point2DMathOpers) {
    Point2D p1{3.0, 4.0};
    Point2D p2{1.0, 2.0};
    EXPECT_EQ(p1 + p2, Point2D(4.0, 6.0));
    EXPECT_EQ(p1 - p2, Point2D(2.0, 2.0));
    EXPECT_EQ(p1 * 2.0, Point2D(6.0, 8.0));
    EXPECT_EQ(p1 / 2.0, Point2D(1.5, 2.0));
}

//геометрические функций точек
TEST(GeometryTest, Point2DGeomOpers) {
    Point2D p1{3.0, 4.0};
    Point2D p2{1.0, 2.0};
    EXPECT_DOUBLE_EQ(p1.Length(), 5.0);
    EXPECT_DOUBLE_EQ(p1.DistanceTo(p2), std::sqrt(4.0 + 4.0));
    Point2D p1_norm = p1.Normalize();
    EXPECT_DOUBLE_EQ(p1_norm.Length(), 1.0);
    EXPECT_DOUBLE_EQ(p1.Dot(p2), 11.0);
    EXPECT_DOUBLE_EQ(p1.Cross(p2), 2.0);
}

// Тест AABB
TEST(GeometryTest, BoundBoxOverlaps) {
    Line l{Point2D{0.0, 5.0}, Point2D{4.0, 1.0}};
    BoundingBox bbox = l.BoundBox();
    EXPECT_DOUBLE_EQ(bbox.min_x, 0.0);
    EXPECT_DOUBLE_EQ(bbox.min_y, 1.0);
    EXPECT_DOUBLE_EQ(bbox.max_x, 4.0);
    EXPECT_DOUBLE_EQ(bbox.max_y, 5.0);
    BoundingBox intersecting_box{3.0, 0.0, 5.0, 2.0};
    BoundingBox distant_box{10.0, 10.0, 12.0, 12.0};

    EXPECT_TRUE(bbox.Overlaps(intersecting_box));
    EXPECT_FALSE(bbox.Overlaps(distant_box));
}
