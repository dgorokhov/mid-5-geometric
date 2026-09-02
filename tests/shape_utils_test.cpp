#include <gtest/gtest.h>
#include "shape_utils.hpp"
#include "geometry.hpp"
#include <vector>

using namespace geometry;

TEST(ShapeUtilsTest, FindAllCollisionsAABB) {
    // Создаем два пересекающихся прямоугольника и один удаленный
    Rectangle r1({0.0, 0.0}, 2.0, 2.0);
    Rectangle r2({1.0, 1.0}, 2.0, 2.0); // Пересекает r1
    Rectangle r3({10.0, 10.0}, 1.0, 1.0); // Далеко

    std::vector<Shape> shapes = {r1, r2, r3};
    auto collisions = utils::FindAllCollisions(shapes);

    // Должна найтись ровно одна пара пересекающихся Bounding Box
    EXPECT_EQ(collisions.size(), 1);
}

TEST(ShapeUtilsTest, FindHighestShape) {
    Rectangle low_rect({0.0, 0.0}, 1.0, 5.0);   // Max Y = 5.0
    Circle high_circle({0.0, 10.0}, 2.0);       // Max Y = 12.0

    std::vector<Shape> shapes = {low_rect, high_circle};
    auto highest_idx = utils::FindHighestShape(shapes);

    ASSERT_TRUE(highest_idx.has_value());
    EXPECT_EQ(*highest_idx, 1); // Индекс окружности
}
