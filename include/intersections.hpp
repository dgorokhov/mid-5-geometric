#pragma once
#include "geometry.hpp"
#include <cmath>
#include <optional>
#include <variant>
#include <stdexcept>

namespace geometry::intersections {

class IntersectionVisitor {
public:
    // 1. Пересечение Line & Line (через косое произведение и уравнения прямых)
    /*
    [[nodiscard]] std::optional<Point2D> operator()(const Line& l1, const Line& l2) const noexcept {
        const double x1 = l1.start.x, y1 = l1.start.y;
        const double x2 = l1.end.x,   y2 = l1.end.y;
        const double x3 = l2.start.x, y3 = l2.start.y;
        const double x4 = l2.end.x,   y4 = l2.end.y;

        const double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (std::abs(denom) < 1e-9) {
            return std::nullopt; // Прямые параллельны или совпадают
        }

        const double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
        const double u = ((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;

        // Если это отрезки, нужно проверить: 0.0 <= t <= 1.0 && 0.0 <= u <= 1.0
        // Но так как тип называется Line (Прямая), считаем пересечение бесконечных прямых.
        // Если тесты потребуют пересечение именно ОТРЕЗКОВ, раскомментируй проверку ниже:
        if (t < 0.0 || t > 1.0 || u < 0.0 || u > 1.0) return std::nullopt;

        return geometry::Point2D{ x1 + t * (x2 - x1), y1 + t * (y2 - y1) };
        return std::make_optional<geometry::Point2D>(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
        return std::make_optional<geometry::Point2D>(geometry::Point2D{x1 + t * (x2 - x1), y1 + t * (y2 - y1)});
        
    }
*/
 [[nodiscard]] std::optional<geometry::Point2D> operator()(const Line& l1, const Line& l2) const noexcept {
        const double x1 = l1.start.x, y1 = l1.start.y;
        const double x2 = l1.end.x,   y2 = l1.end.y;
        const double x3 = l2.start.x, y3 = l2.start.y;
        const double x4 = l2.end.x,   y4 = l2.end.y;

        const double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (std::abs(denom) < 1e-9) {
            return std::nullopt;
        }

        const double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
        
        // Жестко и явно собираем точку и заворачиваем в optional
        return std::optional<geometry::Point2D>{ geometry::Point2D{ x1 + t * (x2 - x1), y1 + t * (y2 - y1) } };
    }

    // 2. Пересечение Line & Circle
    [[nodiscard]] std::optional<Point2D> operator()(const Line& l, const Circle& c) const noexcept {
        const Point2D d = l.end - l.start;
        const Point2D f = l.start - c.center_p;

        const double a = d.Dot(d);
        const double b = 2.0 * f.Dot(d);
        const double cc = f.Dot(f) - c.radius * c.radius;

        const double discriminant = b * b - 4.0 * a * cc;
        if (discriminant < 0.0) {
            return std::nullopt; // Нет точек пересечения
        }

        // Находим ближайшую точку (первую по направлению луча)
        const double t = (-b - std::sqrt(discriminant)) / (2.0 * a);
        
        // Аналогично: если это отрезок, проверяем границы t (0.0 <= t <= 1.0)
        // Если это бесконечная прямая, то возвращаем точку в любом случае.
        return l.start + d * t;
    }

    // Кросс-кейс: Circle & Line (просто меняем аргументы местами и вызываем верхний метод)
    [[nodiscard]] std::optional<Point2D> operator()(const Circle& c, const Line& l) const noexcept {
        return (*this)(l, c);
    }

    // 3. Пересечение Circle & Circle
    [[nodiscard]] std::optional<Point2D> operator()(const Circle& c1, const Circle& c2) const noexcept {
        const double d = c1.center_p.DistanceTo(c2.center_p);
        
        // Нет пересечений или одна окружность внутри другой, либо совпадают
        if (d > c1.radius + c2.radius || d < std::abs(c1.radius - c2.radius) || d < 1e-9) {
            return std::nullopt;
        }

        const double a = (c1.radius * c1.radius - c2.radius * c2.radius + d * d) / (2.0 * d);
        const Point2D p2 = c1.center_p + (c2.center_p - c1.center_p) * (a / d);
        
        // Возвращаем одну из двух точек пересечения хорды и линии центров
        return p2;
    }

    // Шаблонный перехватчик: генерирует ошибку для всех остальных неразрешенных комбинаций фигур
    template <typename T, typename U>
    std::optional<Point2D> operator()(const T&, const U&) const {
        throw std::logic_error("Данная комбинация фигур не поддерживается для поиска точек пересечения!");
    }
};

// Функция-диспетчер: разворачивает std::variant и запускает нужный метод посетителя
inline std::optional<Point2D> GetIntersectPoint(const Shape &shape1, const Shape &shape2) {
    return std::visit(IntersectionVisitor{}, shape1, shape2);
}

}  // namespace geometry::intersections
