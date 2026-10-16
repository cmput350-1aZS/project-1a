
#ifndef BEZIER_H
#define BEZIER_H

#include <vector>
#include "MathUtil.h"

namespace CMPUT350 {

class Bezier {
public:
    // stores the control points for the curve.
    Bezier(const std::vector<Point2D>& pts);

    // returns the point on the curve at time t
    Point2D GetPoint(float t) const;

    // returns the slope of the curve at time t
    Point2D GetSlope(float t) const;

    // calculates point without creating a Bezier object
    static Point2D GetPoint(const std::vector<Point2D>& pts, float t);

    // calculates a slope without creating a Bezier object
    static Point2D GetSlope(const std::vector<Point2D>& pts, float t);

private:
    std::vector<Point2D> mPoints;
};

} // namespace CMPUT350

#endif
