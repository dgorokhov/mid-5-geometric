#pragma once
#include "geometry.hpp"
#include <algorithm>
#include <ranges>
#include <vector>
#include <span>
#include <expected>
#include <string_view>

namespace geometry::convex_hull {

// Вспомогательная функция для определения поворота (знак косого произведения)
[[nodiscard]] inline double CrossProduct(Point2D p1, Point2D middle, Point2D p2) noexcept {
    return (middle.x - p1.x) * (p2.y - p1.y) - (middle.y - p1.y) * (p2.x - p1.x);
}

class StackForGrahamScan {
public:
    constexpr void Push(const Point2D &p) { s.push_back(p); }
    constexpr void Pop() { s.pop_back(); }

    [[nodiscard]] constexpr size_t Size() const noexcept { return s.size(); }
    [[nodiscard]] constexpr Point2D Top() const noexcept { return s.back(); }
    [[nodiscard]] constexpr Point2D NextToTop() const noexcept { return *std::prev(s.end(), 2); }

    [[nodiscard]] std::vector<Point2D> Extract() noexcept { return std::move(s); }

private:
    std::vector<Point2D> s;
};

// Перечисление возможных ошибок алгоритма Грэхема
enum class ConvexHullError {
    TooFewPoints,      // Меньше 3 уникальных точек
    DegenerateGeometry // Все точки лежат на одной прямой
};

// Пункт 11: Безопасный алгоритм Грэхема на std::expected и C++20 Ranges
[[nodiscard]] inline std::expected<std::vector<Point2D>, ConvexHullError> 
GrahamScan(std::span<Point2D> points) noexcept {

    if (points.size() < 3) {
        return std::unexpected(ConvexHullError::TooFewPoints);
    }

    // 1. Находим самую нижнюю (а затем самую левую) точку с помощью std::ranges
    auto min_it = std::ranges::min_element(points, [](const Point2D &a, const Point2D &b) noexcept {
        if (a.y != b.y) return a.y < b.y;
        return a.x < b.x;
    });

    // Меняем её местами с первой точкой в массиве
    std::swap(points[0], *min_it);
    Point2D p0 = points[0];

    // 2. Сортируем остальные точки по полярному углу относительно p0 с помощью Ranges
    std::ranges::sort(points.subspan(1), [p0](const Point2D &a, const Point2D &b) noexcept {
        double order = CrossProduct(p0, a, b);
        if (std::abs(order) < EPSILON_GEOM) {
            return p0.DistanceTo(a) < p0.DistanceTo(b);
        }
        return order > 0.0;
    });

    // 3. Обходим точки и строим оболочку через кастомный стек
    StackForGrahamScan stack;
    stack.Push(points[0]);
    stack.Push(points[1]);
    stack.Push(points[2]);

    for (size_t i = 3; i < points.size(); ++i) {
        while (stack.Size() >= 2 && CrossProduct(stack.NextToTop(), stack.Top(), points[i]) <= 0.0) {
            stack.Pop();
        }
        stack.Push(points[i]);
    }
    // Проверяем вырожденный случай (если все точки оказались на одной прямой, размер оболочки < 3)
    if (stack.Size() < 3) {
        return std::unexpected(ConvexHullError::DegenerateGeometry);
    }

    return stack.Extract();
}

}  // namespace geometry::convex_hull
