#include <string>
#include "queries.hpp"
#include "visualization.hpp"
#include "geometry.hpp"

#include <matplot/matplot.h>
#include <print>

namespace geometry::visualization {

template <class... Ts>
struct Multilambda : Ts... {
    using Ts::operator()...;
};
auto DrawConfig()
{
    using namespace geometry;
    using namespace matplot;

    // Disable gnuplot warnings
    auto f = figure(false);
    f->backend()->run_command("unset warnings");
    f->ioff();
    f->size(900, 900);

    hold(on);     // Multiple plots mode
    axis(equal);  // Squre view
    grid(on);     // Enable grid by default
    return f;
}

void Draw(std::span<geometry::Shape> shapes) {
    using namespace geometry;
    using namespace matplot;
    const auto& fh = DrawConfig();

    for (const auto &[index, shape] : std::ranges::views::enumerate(shapes)) {
        // Отрисовка линий для каждого типа фигуры через Multilambda и std::visit
        std::visit(geometry::queries::Multilambda {
            [](const Line &l) { plot(l.Lines().x, l.Lines().y)->line_width(2).color("yellow"); },
            [](const Triangle &t) { plot(t.Lines().x, t.Lines().y)->line_width(2).color("blue"); },
            [](const Rectangle &r) { plot(r.Lines().x, r.Lines().y)->line_width(2).color("green"); },
            [](const RegularPolygon &rp) { plot(rp.Lines().x, rp.Lines().y)->line_width(2).color("magenta"); },
            [](const Circle &c) { plot(c.Lines(100).x, c.Lines(100).y)->line_width(2).color("red"); },
            [](const Polygon &p) { plot(p.Lines().x, p.Lines().y)->line_width(2).color("cyan"); }
        }, shape);

        //получение центра для добавления номера фигуры
        const auto center = std::visit([](auto &&s) { return s.Center(); }, shape);
        
        auto t = text(center.x, center.y, std::to_string(index));
        t->font_size(14);
        t->color("black");
    }

    //сохранение графиков на диск по шагам
    static int step_counter = 1;
    std::string file_name = "shapes_" + std::to_string(step_counter++) + "_graph.png";
    fh->save(file_name);
}

void Draw(std::span<geometry::triangulation::DelaunayTriangle> triangles) {
    using namespace geometry;
    using namespace matplot;
    
    const auto& fh = DrawConfig();

    for (const auto &[index, d_triangle] : std::ranges::views::enumerate(triangles)) {
        const geometry::Triangle tri{d_triangle.a, d_triangle.b, d_triangle.c};
        const auto lines = tri.Lines();
        plot(lines.x, lines.y)->line_width(2).color("cyan");

        // Add triangle number
        const auto center = tri.Center();
        auto t = text(center.x, center.y, std::to_string(index));
        t->font_size(14);
        t->color("black");
    }

    // Display plot
    //fh->show();
    static int step_counter = 1;
    std::string file_name = "triangle_" + std::to_string(step_counter++) + "_graph.png";
    fh->save(file_name);
}

}  // namespace geometry::visualization
