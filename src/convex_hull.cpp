#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <numbers>
#include <ranges>
#include <variant>
#include <vector>
#include <string_view>
#include <iterator>

namespace geometry {
struct Point2D {
    double x, y;

    [[nodiscard]] constexpr Point2D() noexcept : x(0), y(0) {}
    [[nodiscard]] constexpr Point2D(double x, double y) noexcept : x(x), y(y) {}

    // Comparison
    [[nodiscard]] constexpr bool operator<(const Point2D &other) const noexcept { return x < other.x && y < other.y; }
    [[nodiscard]] constexpr bool operator==(const Point2D &other) const noexcept {
        return x == other.x && y == other.y;
    }

    // Binary math operators
    [[nodiscard]] constexpr Point2D operator+(const Point2D &other) const noexcept {
        return {x + other.x, y + other.y};
    }
    [[nodiscard]] constexpr Point2D operator-(const Point2D &other) const noexcept {
        return {x - other.x, y - other.y};
    }
    [[nodiscard]] constexpr Point2D operator*(double value) const noexcept { return {x * value, y * value}; }
    [[nodiscard]] constexpr Point2D operator/(double value) const noexcept { return {x / value, y / value}; }

    // Binary geometry operations
    [[nodiscard]] constexpr double Dot(const Point2D &other) const noexcept { return x * other.x + y * other.y; }
    [[nodiscard]] constexpr double Cross(const Point2D &other) const noexcept { return x * other.y - y * other.x; }
    [[nodiscard]] inline double Length() const noexcept { return std::sqrt(x * x + y * y); }
    [[nodiscard]] inline double DistanceTo(const Point2D &other) const noexcept { return (*this - other).Length(); }

    [[nodiscard]] inline Point2D Normalize() const noexcept {
        const double len = Length();
        return len > 0 ? Point2D{x / len, y / len} : Point2D{0, 0};
    }
};

template <size_t N>
struct Lines2D {
    std::array<double, N> x;
    std::array<double, N> y;
};

struct Lines2DDyn {
    std::vector<double> x;
    std::vector<double> y;

    constexpr void Reserve(size_t n) {
        x.reserve(n);
        y.reserve(n);
    }
    constexpr void PushBack(Point2D p) {
        x.push_back(p.x);
        y.push_back(p.y);
    }
    constexpr void PushBack(double px, double py) {
        x.push_back(px);
        y.push_back(py);
    }
    [[nodiscard]] constexpr Point2D Front() const { return {x.front(), y.front()}; }
};
struct BoundingBox {
    double min_x, min_y, max_x, max_y;

    constexpr BoundingBox() noexcept : min_x(0), min_y(0), max_x(0), max_y(0) {}
    constexpr BoundingBox(double min_x, double min_y, double max_x, double max_y) noexcept
        : min_x(min_x), min_y(min_y), max_x(max_x), max_y(max_y) {}

    [[nodiscard]] constexpr bool Overlaps(const BoundingBox &other) const noexcept {
        return !(max_x < other.min_x || min_x > other.max_x || max_y < other.min_y || min_y > other.max_y);
    }

    [[nodiscard]] constexpr double Width() const noexcept { return max_x - min_x; }
    [[nodiscard]] constexpr double Height() const noexcept { return max_y - min_y; }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return {(min_x + max_x) / 2, (min_y + max_y) / 2}; }
};

struct Line {
    Point2D start, end;

    constexpr Line(Point2D start, Point2D end) noexcept : start(start), end(end) {}

    [[nodiscard]] inline double Length() const noexcept { return start.DistanceTo(end); }
    [[nodiscard]] inline Point2D Direction() const noexcept { return (end - start).Normalize(); }
    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {std::min(start.x, end.x), std::min(start.y, end.y), std::max(start.x, end.x), std::max(start.y, end.y)};
    }
    [[nodiscard]] constexpr double Height() const noexcept { return std::max(start.y, end.y); }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return (start + end) / 2.0; }

    [[nodiscard]] constexpr std::array<Point2D, 2> Vertices() const noexcept {
        return {Point2D{start.x, start.y}, {end.x, end.y}};
    }
    [[nodiscard]] constexpr Lines2D<2> Lines() const noexcept { return {{start.x, end.x}, {start.y, end.y}}; }
};

struct Triangle {
    Point2D a, b, c;

    constexpr Triangle(Point2D a, Point2D b, Point2D c) noexcept : a(a), b(b), c(c) {}

    [[nodiscard]] constexpr double Area() const noexcept { return std::abs((b - a).Cross(c - a)) / 2.0; }
    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {std::min({a.x, b.x, c.x}), std::min({a.y, b.y, c.y}), std::max({a.x, b.x, c.x}),
                std::max({a.y, b.y, c.y})};
    }
    [[nodiscard]] constexpr std::array<Point2D, 3> Vertices() const noexcept { return {a, b, c}; }
    [[nodiscard]] constexpr double Height() const noexcept { return std::max({a.y, b.y, c.y}); }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return (a + b + c) / 3.0; }

    [[nodiscard]] constexpr Lines2D<4> Lines() const noexcept { return {{a.x, b.x, c.x, a.x}, {a.y, b.y, c.y, a.y}}; }
};

struct Rectangle {
    Point2D bottom_left;
    double width, height;

    constexpr Rectangle(Point2D bottom_left, double width, double height) noexcept
        : bottom_left(bottom_left), width(width), height(height) {}

    [[nodiscard]] constexpr Point2D TopRight() const noexcept {
        return {bottom_left.x + width, bottom_left.y + height};
    }
    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {bottom_left.x, bottom_left.y, bottom_left.x + width, bottom_left.y + height};
    }
    [[nodiscard]] constexpr std::array<Point2D, 4> Vertices() const noexcept {
        return {bottom_left,
                {bottom_left.x + width, bottom_left.y},
                {bottom_left.x + width, bottom_left.y + height},
                {bottom_left.x, bottom_left.y + height}};
    }
    [[nodiscard]] constexpr double Height() const noexcept { return bottom_left.y + height; }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return bottom_left + (Point2D{width, height} / 2.0); }

    [[nodiscard]] constexpr Lines2D<5> Lines() const noexcept {
        return {{bottom_left.x, bottom_left.x, bottom_left.x + width, bottom_left.x + width, bottom_left.x},
                {bottom_left.y, bottom_left.y + height, bottom_left.y + height, bottom_left.y, bottom_left.y}};
    }
};

struct RegularPolygon {
    Point2D center_p;
    double radius;
    int sides;

    constexpr RegularPolygon(Point2D center, double radius, int sides)
        : center_p(center), radius(radius), sides(sides) {}

    std::vector<Point2D> Vertices() const {
        std::vector<Point2D> points;
        points.reserve(sides);

        for (int i = 0; i < sides; ++i) {
            const double angle = 2 * std::numbers::pi * i / sides;
            points.emplace_back(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        return points;
    }

    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }
    [[nodiscard]] constexpr double Height() const noexcept { return center_p.y + radius; }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return center_p; }

    [[nodiscard]] Lines2DDyn Lines() {
        auto verts = Vertices();
        Lines2DDyn lines;
        lines.Reserve(verts.size() + 1);
        for (const auto &p : verts) {
            lines.PushBack(p);
        }
        lines.PushBack(lines.Front());
        return lines;
    }
};

struct Circle {
    Point2D center_p;
    double radius;

    constexpr Circle(Point2D center, double radius) noexcept : center_p(center), radius(radius) {}

    [[nodiscard]] constexpr BoundingBox BoundBox() const noexcept {
        return {center_p.x - radius, center_p.y - radius, center_p.x + radius, center_p.y + radius};
    }
    [[nodiscard]] constexpr double Height() const noexcept { return center_p.y + radius; }
    [[nodiscard]] constexpr Point2D Center() const noexcept { return center_p; }

    [[nodiscard]] std::vector<Point2D> Vertices(size_t N = 30) const {
        std::vector<Point2D> points;
        points.reserve(N);

        for (auto i : std::ranges::views::iota(0u, N)) {
            const double angle = 2 * std::numbers::pi * i / N;
            points.emplace_back(center_p.x + radius * std::cos(angle), center_p.y + radius * std::sin(angle));
        }
        return points;
    }
    [[nodiscard]] Lines2DDyn Lines(size_t N = 100) const {
        auto verts = Vertices(N);
        Lines2DDyn lines;
        lines.Reserve(verts.size() + 1);
        for (const auto &p : verts) {
            lines.PushBack(p);
        }
        lines.PushBack(lines.Front());
        return lines;
    }
};
} // namespace geometry
// Этот код должен находиться в самом низу файла geometry.hpp, 
// строго ЗА пределами namespace geometry { ... }

template <>
struct std::formatter<std::vector<geometry::Point2D>> {
    // Внутренний флаг структуры — теперь он точно будет виден в функциях ниже
    bool is_new_line = false;

    constexpr auto parse(std::format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(); // <-- ВОТ ЭТА СТРОКА ПРОПАЛА НА СКРИНШОТЕ
        if (it == ctx.end() || *it == '}') {
            return it;
        }

        // Читаем спецификатор форматирования
        std::string_view view(&*it, std::distance(it, ctx.end()));
        if (view.starts_with("new_line")) {
            is_new_line = true;
            std::advance(it, std::string_view("new_line").size()); // Сдвигаем итератор на длину строки
        }

        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid specifier");
        }
        return it;
    }

    auto format(const std::vector<geometry::Point2D>& points, std::format_context& ctx) const -> decltype(ctx.out()) {
        auto out = ctx.out();
        if (points.empty()) {
            return std::format_to(out, "[]");
        }

        out = std::format_to(out, "[");
        for (size_t i = 0; i < points.size(); ++i) {
            if (is_new_line) {
                out = std::format_to(out, "\n\t({:.2f}, {:.2f})", points[i].x, points[i].y);
            } else {
                out = std::format_to(out, "({:.2f}, {:.2f})", points[i].x, points[i].y);
                if (i + 1 < points.size()) {
                    out = std::format_to(out, ", ");
                }
            }
        }
        
        if (is_new_line) {
            out = std::format_to(out, "\n]");
        } else {
            out = std::format_to(out, "]");
        }
        return out;
    }
};
