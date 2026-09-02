#include <gtest/gtest.h>
#include "queries.hpp"
#include "geometry.hpp"

using namespace geometry;

TEST(QueriesTest, PointToCircleDistance) {
    Circle c({0.0, 0.0}, 3.0);
    Point2D p{5.0, 0.0};

    // Расстояние от центра до точки = 5. Минус радиус 3 = 2.
    double dist = queries::DistanceToPoint(c, p);
    EXPECT_DOUBLE_EQ(dist, 2.0);
}

TEST(QueriesTest, PointInsideCircleDistance) {
    Circle c({0.0, 0.0}, 3.0);
    Point2D p{1.0, 0.0};

    // Точка внутри круга, расстояние должно быть строго 0
    double dist = queries::DistanceToPoint(c, p);
    EXPECT_DOUBLE_EQ(dist, 0.0);
}
