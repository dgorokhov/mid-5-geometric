#include <span>
#include <optional>
#include <vector>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <string_view>

#include "queries.hpp"

namespace geometry::utils {


// Пункт 8: Реализация алгоритмов анализа набора фигур
// Функция поиска всех пересечений между фигурами методом Bounding Box
[[nodiscard]] std::vector<std::pair<Shape, Shape>> FindAllCollisions(std::span<const Shape> shapes) {
    std::vector<std::pair<Shape, Shape>> collisions;
    if (shapes.size() < 2) return collisions;

    // Перебираем все уникальные пары фигур (O(N^2))
    for (size_t i = 0; i < shapes.size(); ++i) {
        for (size_t j = i + 1; j < shapes.size(); ++j) {
            // Используем созданную в queries.hpp функцию BoundingBoxesOverlap
            if (queries::BoundingBoxesOverlap(shapes[i], shapes[j])) {
                collisions.emplace_back(shapes[i], shapes[j]);
            }
        }
    }
    return collisions;
}

// Функция для поиска индекса самой высокой фигуры (по максимальной Y-координате)
[[nodiscard]] std::optional<size_t> FindHighestShape(std::span<const Shape> shapes) {
    if (shapes.empty()) {
        return std::nullopt;
    }

    size_t highest_index = 0;
    double max_height = queries::GetHeight(shapes[0]);

    for (size_t i = 1; i < shapes.size(); ++i) {
        double current_height = queries::GetHeight(shapes[i]);
        if (current_height > max_height) {
            max_height = current_height;
            highest_index = i;
        }
    }
    return highest_index;
}


// Пункты 9 и 10: Монадическая фабрика геометрических объектов
// Вспомогательная структура для парсинга параметров (пример реализации)
struct RawData {
    std::vector<double> values;
};

// Имитация функции парсинга строки, возвращающей optional с данными
std::optional<RawData> ExtractRawData(std::string_view input) {
    // В реальном коде здесь будет разбор строки. Пока возвращаем пустой заглушечный результат.
    return RawData{};
}

// Реализация MakeCircle через монады (.transform возвращает optional<Shape>)
std::optional<Shape> MakeCircle(std::string_view input) {
    return ExtractRawData(input)
        .transform([](const RawData& data) -> Shape {
            // Предположим, первые 2 числа — центр, 3-е — радиус
            Point2D center{data.values.size() > 0 ? data.values[0] : 0.0, 
                           data.values.size() > 1 ? data.values[1] : 0.0};
            double radius = data.values.size() > 2 ? data.values[2] : 1.0;
            return Circle{center, radius};
        });
}

std::optional<Shape> MakeLine(std::string_view input) {
    return ExtractRawData(input)
        .transform([](const RawData& data) -> Shape {
            Point2D start{0.0, 0.0};
            Point2D end{1.0, 1.0};
            return Line{start, end};
        });
}

std::optional<Shape> MakeTriangle(std::string_view input) {
    return ExtractRawData(input)
        .transform([](const RawData& data) -> Shape {
            return Triangle{Point2D{0,0}, Point2D{1,0}, Point2D{0,1}};
        });
}

std::optional<Shape> MakeRectangle(std::string_view input) {
    return ExtractRawData(input)
        .transform([](const RawData& data) -> Shape {
            return Rectangle{Point2D{0,0}, 1.0, 1.0};
        });
}

std::optional<Shape> MakePolygon(std::string_view input) {
    return ExtractRawData(input)
        .transform([](const RawData& data) -> Shape {
            return RegularPolygon{Point2D{0,0}, 1.0, 5};
        });
}

// Пункт 10: Монадический диспетчер парсинга одной фигуры
std::optional<Shape> ParseSingleShape(std::string_view type, std::string_view params) {
    if (type == "circle")    return MakeCircle(params);
    if (type == "line")      return MakeLine(params);
    if (type == "triangle")  return MakeTriangle(params);
    if (type == "rectangle") return MakeRectangle(params);
    if (type == "polygon")   return MakePolygon(params);
    
    return std::nullopt; // Если тип неизвестен, летит пустой optional
}

// Главный парсер всего инпута
std::vector<Shape> ParseShapes(std::string_view input) {
    std::vector<Shape> result;
    // Логика разбиения строки на токены и вызов ParseSingleShape
    // ...
    return result;
}

}  // namespace geometry::utils
