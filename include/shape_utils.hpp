#include <span>
#include <optional>
#include <vector>
#include <utility>
#include <string_view>
#include <sstream>
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

// Реальная функция парсинга параметров из строки
std::optional<RawData> ExtractRawData(std::string_view input) {
    RawData data;
    
    // Создаем поток из строки для удобного чтения чисел через оператор >>
    std::stringstream ss((std::string(input)));
    double value;
    
    // Читаем все числа, разделенные пробелами, пока поток не закончится
    while (ss >> value) {
        data.values.push_back(value);
    }
    
    // Если мы ничего не смогли распарсить (или строка была пустой/битой), 
    // возвращаем nullopt, чтобы сработала монадическая защита
    if (data.values.empty()) {
        return std::nullopt;
    }
    
    return data;
}
/*
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

// Реализуем главный парсер: он бьет строку по ';' и вызывает твой ParseSingleShape
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

       //монадический диспетчер
        auto shape_opt = ParseSingleShape(type, clean_segment);
        if (shape_opt.has_value()) {
            result.push_back(shape_opt.value());
        }
    }
    return result;
}

*/


// Обновленный монадический диспетчер парсинга одной фигуры
std::optional<Shape> ParseSingleShape(std::string_view type, std::string_view params) {
    auto data_opt = ExtractRawData(params);
    if (!data_opt.has_value()) return std::nullopt;
    
    const auto& data = data_opt.value();

    if (type == "circle" && data.values.size() >= 3) {
        double radius = data.values[2];
        if (radius < 0) return std::nullopt; // Игнорируем отрицательный радиус по ТЗ
        return Circle{Point2D{data.values[0], data.values[1]}, radius};
    }
    if (type == "line" && data.values.size() >= 4) {
        return Line{Point2D{data.values[0], data.values[1]}, Point2D{data.values[2], data.values[3]}};
    }
    if (type == "triangle" && data.values.size() >= 6) {
        return Triangle{Point2D{data.values[0], data.values[1]}, 
                        Point2D{data.values[2], data.values[3]}, 
                        Point2D{data.values[4], data.values[5]}};
    }
    if (type == "rectangle" && data.values.size() >= 3) {
        double w = data.values[2];
        double h = data.values.size() >= 4 ? data.values[3] : w; // если не задана высота, делаем квадрат
        return Rectangle{Point2D{data.values[0], data.values[1]}, w, h};
    }
    if (type == "polygon" && data.values.size() >= 3 && data.values[3] >= 4 ) {
        return RegularPolygon{Point2D{data.values[0], data.values[1]}, data.values[2], 
                              static_cast<int>(data.values[3])};
    }
    
    return std::nullopt; 
}

// РЕАЛЬНАЯ функция разбиения строки на токены через C++20 Ranges
std::vector<Shape> ParseShapes(std::string_view input) {
    std::vector<Shape> result;

    // Разбиваем строку на подстроки по разделителю ';' с помощью C++20 views::split
    auto tokens = input | std::views::split(';');

    for (auto token : tokens) {
        // Конвертируем subrange обратно в string_view
        std::string_view token_view(token.data(), token.size());
        
        // Убираем лишние пробелы в начале, если они есть
        while (!token_view.empty() && token_view.front() == ' ') {
            token_view.remove_prefix(1);
        }
        if (token_view.empty()) continue;

        // Ищем первый пробел, разделяющий тип фигуры и её параметры
        size_t space_pos = token_view.find(' ');
        if (space_pos == std::string_view::npos) {
            continue; // Пропускаем битые строки вроде "badshape" без параметров
        }

        std::string_view type = token_view.substr(0, space_pos);
        std::string_view params = token_view.substr(space_pos + 1);

        // Монадически пытаемся распарсить фигуру
        auto single_shape = ParseSingleShape(type, params);
        if (single_shape.has_value()) {
            result.push_back(single_shape.value());
        }
    }

    return result;
}

}  // namespace geometry::utils
