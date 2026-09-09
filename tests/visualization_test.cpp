#include <gtest/gtest.h>
#include "visualization.hpp"
#include "geometry.hpp"
#include <vector>
#include <fstream>
#include <filesystem>

using namespace geometry;
/*
TEST(VisualizationTest, HeadlessDrawAndSavePNG) {
    std::vector<Shape> test_shapes = {
        Circle{Point2D{0.0, 0.0}, 1.5},
        Line{Point2D{1.0, 2.0}, Point2D{3.0, 4.0}}
    };

    const std::string filename = "shapes_1_graph.png";
    if (std::filesystem::exists(filename)) {
        std::filesystem::remove(filename);
    }
    ASSERT_FALSE(std::filesystem::exists(filename));
    EXPECT_NO_THROW(visualization::Draw(test_shapes));
    EXPECT_TRUE(std::filesystem::exists(filename));
}
    
    TEST(VisualizationTest, HeadlessDrawAndSavePNG) {
    std::vector<Shape> test_shapes = {
        Circle{Point2D{0.0, 0.0}, 1.5},
        Line{Point2D{1.0, 2.0}, Point2D{3.0, 4.0}}
    };

    // 1. Используем системную временную директорию (гарантирует права на запись в Docker)
    std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
    std::filesystem::path file_path = temp_dir / "shapes_1_graph.png";

    // 2. Чистим старый файл, если он остался
    if (std::filesystem::exists(file_path)) {
        std::filesystem::remove(file_path);
    }
    ASSERT_FALSE(std::filesystem::exists(file_path));

    // 3. Вызываем отрисовку
    // ВНИМАНИЕ: Если функция Draw принимает путь, передай ей file_path.string()!
    // Если путь жестко зашит внутри Draw, нам нужно временно сменить рабочую директорию (см. ниже).
    EXPECT_NO_THROW(visualization::Draw(test_shapes));

    // 4. Проверяем наличие файла по конкретному пути
    EXPECT_TRUE(std::filesystem::exists(file_path));
}
*/

#include <gtest/gtest.h>
#include <filesystem>
#include <vector>

TEST(VisualizationTest, HeadlessDrawAndSavePNG) {
    std::vector<Shape> test_shapes = {
        Circle{Point2D{0.0, 0.0}, 1.5},
        Line{Point2D{1.0, 2.0}, Point2D{3.0, 4.0}}
    };

    // 1. Получаем текущую рабочую директорию процесса (где бы в Docker мы ни находились)
    std::filesystem::path current_dir = std::filesystem::current_path();
    std::filesystem::path file_path = current_dir / "shapes_1_graph.png";

    // 2. Чистим старый файл, если он остался от прошлых запусков
    if (std::filesystem::exists(file_path)) {
        std::filesystem::remove(file_path);
    }
    ASSERT_FALSE(std::filesystem::exists(file_path));

    // 3. Вызываем отрисовку — matplot сохранит файл именно в current_dir
    EXPECT_NO_THROW(visualization::Draw(test_shapes));

    // 4. Проверяем наличие файла по абсолютному пути процесса
    EXPECT_TRUE(std::filesystem::exists(file_path));
}
