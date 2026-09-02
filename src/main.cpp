
#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"

#include <algorithm>
#include <print>
#include <ranges>
#include <vector>
#include <iostream>
#include <string>

using namespace geometry;

namespace rng = std::ranges;
namespace views = std::ranges::views;

// Объявление паттерна "мультилямбда" для работы std::visit с variant
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void PrintAllIntersections(const Shape &shape, std::span<const Shape> others) {
    std::println("\n=== Intersections ===");
    // Оставляем только те фигуры, которые поддерживают вычисление пересечений (Line и Circle)
    //остальные комбинации вызывают std::logic_error, отлавливаем их или фильтруем
    for (const auto &other : others) {
        try {
            auto intersect_pt = intersections::GetIntersectPoint(shape, other);
            if (intersect_pt.has_value()) {
                std::println("Пересечение найдено в точке ({:.2f}, {:.2f})", 
                             intersect_pt->x, intersect_pt->y);
            } else {
                std::println("Фигуры не пересекаются");
            }
        } catch (const std::logic_error &) {
           
        }
    }
}

void PrintDistancesFromPointToShapes(Point2D p, std::span<const Shape> shapes) {
    std::println("\n=== Distance from Point Test ===");

    // Выбираем первые 5 фигур из списка при помощи C++20 Ranges Views
    for (const auto &shape : shapes | views::take(5)) {
        double dist = queries::DistanceToPoint(shape, p);
        std::println("Расстояние от точки ({:.2f}, {:.2f}) до фигуры равно {:.2f}", p.x, p.y, dist);
    }
}

void PerformShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Analysis ===");

    //Поиск коллизий методом Bounding Box AABB
    auto collisions = utils::FindAllCollisions(shapes);
    std::println("Найдено коллизий по Bounding Box: {}", collisions.size());

    //Поиск индекса самой высокой фигуры
    auto highest_idx = utils::FindHighestShape(shapes);
    if (highest_idx.has_value()) {
        std::println("Самая высокая фигура имеет индекс: {} (высота: {:.2f})", 
                     *highest_idx, queries::GetHeight(shapes[*highest_idx]));
    }

    //Расстояние между любыми двумя фигурами, поддерживающими операцию
    for (size_t i = 0; i < shapes.size(); ++i) {
        for (size_t j = i + 1; j < shapes.size(); ++j) {
            auto dist = queries::DistanceBetweenShapes(shapes[i], shapes[j]);
            if (dist.has_value()) {
                std::println("Расстояние между фигурой {} и {} равно {:.2f}", i, j, *dist);
            }
        }
    }
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");

    //Фильтруем и выводим первые 3 любые фигуры, которые находятся выше 50.0 по Y
    auto high_shapes = shapes 
        | views::filter([](const Shape &s) noexcept { return queries::GetHeight(s) > 50.0; })
        | views::take(3);

    size_t count = 0;
    for (const auto &s : high_shapes) {
        std::println("Фигура выше 50.0 найдена, её высота: {:.2f}", queries::GetHeight(s));
        count++;
    }
    if (count == 0) {
        std::println("Фигур с высотой > 50.0 в наборе не обнаружено.");
    }

    //фигуры с наименьшей и наибольшей высотами
    if (!shapes.empty()) {
        auto [min_it, max_it] = rng::minmax_element(shapes, [](const Shape &a, const Shape &b) noexcept {
            return queries::GetHeight(a) < queries::GetHeight(b);
        });
        std::println("Наименьшая высота: {:.2f}", queries::GetHeight(*min_it));
        std::println("Наибольшая высота: {:.2f}", queries::GetHeight(*max_it));
    }
}


int main() {
    std::vector<Shape> shapes = 
    utils::ParseShapes("circle 0 0 1.5; line 1 2 3 4; polygon 0 0 2 5; triangle 0 0 1 0 0.5 1; polygon 0 0 1 2; badshape; circle 0 0 -1");

    std::println("Parsed {} shapes", shapes.size());

    // Выведите индекс каждой фигуры и её высоту
    for (size_t i = 0; i < shapes.size(); ++i) {
        std::println("Индекс фигуры: {}, Высота (max Y): {:.2f}", i, queries::GetHeight(shapes[i]));
    }

    // Вызываем разработанные аналитические функции
    PrintAllIntersections(shapes[0], shapes);
    PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);
    PerformShapeAnalysis(shapes);
    PerformExtraShapeAnalysis(shapes);

    // Рисуем первый график (все распарсенные фигуры)
    geometry::visualization::Draw(shapes);

    std::println("\nНажмите Enter, чтобы построить второй график (Выпуклая оболочка)...");
    std::cin.get();
    std::vector<Point2D> points;
    for (const auto &shape : shapes) {
        std::visit(overloaded {
            [&points](const Line &l) { points.push_back(l.start); points.push_back(l.end); },
            [&points](const Circle &c) { for (const auto &p : c.Vertices(15)) points.push_back(p); },
            [&points](const Triangle &t) { points.push_back(t.a); points.push_back(t.b); points.push_back(t.c); },
            [&points](const Rectangle &r) { for (const auto &p : r.Vertices()) points.push_back(p); },
            [&points](const RegularPolygon &rp) { for (const auto &p : rp.Vertices()) points.push_back(p); },
            [&points](const Polygon &poly) { for (const auto &p : poly.Vertices()) points.push_back(p); },
            [](const BoundingBox &) {} // У BB вершины опускаем или берем углы при необходимости
        }, shape);
    }
    // построение выпуклой оболочки алгоритмом Грэхема
    auto hull_result = convex_hull::GrahamScan(points);
    if (hull_result.has_value()) {
        std::vector<Point2D> hull_vertices = hull_result.value();
        Polygon hull_poly{hull_vertices};
        shapes.push_back(hull_poly);
        geometry::visualization::Draw(shapes);
    } else {
        std::println("Не удалось построить выпуклую оболочку: мало точек или вырожденная геометрия.");
    }
    std::println("\nНажмите Enter, чтобы построить третий график (Триангуляция Делоне)...");
    std::cin.get();

    {
        std::vector<Point2D> tri_points = {{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};
        auto tri_result = triangulation::DelaunayTriangulation(tri_points);
        
        if (tri_result.has_value()) {
            std::println("Триангуляция Делоне успешно построена. Количество треугольников: {}", tri_result->size());
            geometry::visualization::Draw(*tri_result);
        } else {
            std::println("Ошибка триангуляции Делоне: недостаточно точек.");
        }
    }

    return 0;
}
