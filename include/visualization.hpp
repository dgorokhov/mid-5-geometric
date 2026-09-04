
#include <span>

#include "geometry.hpp"
#include "triangulation.hpp"


namespace geometry::visualization {

void Draw(std::span<geometry::Shape> shapes);

void Draw(std::span<geometry::triangulation::DelaunayTriangle> triangles);

}  // namespace geometry::visualization
