#include <gtest/gtest.h>
#include "visualization.hpp"
#include "geometry.hpp"
#include <vector>
#include <fstream>

using namespace geometry;

TEST(VisualizationTest, HeadlessDrawAndSavePNG) {
    std::vector<Shape> test_shapes = {
        Circle{Point2D{0.0, 0.0}, 1.5},
        Line{Point2D{1.0, 2.0}, Point2D{3.0, 4.0}}
    };

    // Проверяем, что функция Draw выполняется без падений рантайма и исключений
    EXPECT_NO_THROW(visualization::Draw(test_shapes));

    // Проверяем, что файл первого шага действительно физически создался на диске
    std::ifstream file("shapes_1_graph.png");
    EXPECT_TRUE(file.good());
    file.close();
}
