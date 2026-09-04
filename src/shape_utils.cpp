#include "queries.hpp"
#include <span>
#include <optional>
#include <vector>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <string_view>
#include <sstream>

namespace geometry::utils {


//алгоритмы анализа набора фигур
// поиск всех пересечений между фигурами методом Bounding Box
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


// Монадическая фабрика объектов
// Вспомогательная структура для парсинга параметров (пример реализации)
struct RawData {
    std::vector<double> values;
};

std::optional<RawData> ExtractRawData(std::string_view input) {
    std::string s(input);
    std::stringstream ss(s);
    
    std::string type;
    ss >> type; // Пропускаем название фигуры (например, "circle")

    RawData data;
    double val;
    // Считываем все идущие следом числа
    while (ss >> val) {
        data.values.push_back(val);
    }
   // плохая строка вроде "badshape" == возвращаем nullopt
    if (data.values.empty() && type == "badshape") {
        return std::nullopt;
    }

    return data;
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

//  Монадический диспетчер парсинга одной фигуры
std::optional<Shape> ParseSingleShape(std::string_view type, std::string_view params) {
    if (type == "circle")    return MakeCircle(params);
    if (type == "line")      return MakeLine(params);
    if (type == "triangle")  return MakeTriangle(params);
    if (type == "rectangle") return MakeRectangle(params);
    if (type == "polygon")   return MakePolygon(params);
    
    return std::nullopt; 
}

// главный парсер: он бьет строку по ';' и вызывает ParseSingleShape
std::vector<Shape> ParseShapes(std::string_view input) {
    std::vector<Shape> result;
    std::string input_str(input);
    std::stringstream ss(input_str);
    std::string segment;

    // Разбиваем строку на подстроки по точке с запятой
    while (std::getline(ss, segment, ';')) {
        // Убираем лишние пробелы в начале
        auto start = segment.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        std::string_view clean_segment = std::string_view(segment).substr(start);

        // Извлекаем тип фигуры для ParseSingleShape (например, до первого пробела)
        std::string segment_str(clean_segment);
        std::stringstream segment_ss(segment_str);
        std::string type;
        segment_ss >> type;

        // Вызываем твой монадический диспетчер
        auto shape_opt = ParseSingleShape(type, clean_segment);
        if (shape_opt.has_value()) {
            result.push_back(shape_opt.value());
        }
    }
    return result;
}

}  // namespace geometry::utils
