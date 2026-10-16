
#include "Bezier.h"

#include <stdexcept>

namespace CMPUT350 {

// checks that the control points form complete cubic segments
static void CheckPoints(const std::vector<Point2D>& pts)
{
    if (pts.size() < 4 || (pts.size() - 1) % 3 != 0) {
        throw std::invalid_argument(
            "Bezier curve requires 3k+1 control points.");
    }
}

// calculates a point on one cubic Bezier segment
static Point2D CalculatePoint(
    const std::vector<Point2D>& pts, size_t start, float t)
{
    const Point2D& p0 = pts[start];
    const Point2D& p1 = pts[start + 1];
    const Point2D& p2 = pts[start + 2];
    const Point2D& p3 = pts[start + 3];

    float u = 1.0f - t;

    float a = u * u * u;
    float b = 3.0f * u * u * t;
    float c = 3.0f * u * t * t;
    float d = t * t * t;

    return Point2D(
        a * p0.x + b * p1.x + c * p2.x + d * p3.x,
        a * p0.y + b * p1.y + c * p2.y + d * p3.y
    );
}

// calculates slope on one cubic Bezier segment
static Point2D CalculateSlope(
    const std::vector<Point2D>& pts, size_t start, float t)
{
    const Point2D& p0 = pts[start];
    const Point2D& p1 = pts[start + 1];
    const Point2D& p2 = pts[start + 2];
    const Point2D& p3 = pts[start + 3];

    float u = 1.0f - t;

    float a = 3.0f * u * u;
    float b = 6.0f * u * t;
    float c = 3.0f * t * t;

    return Point2D(
        a * (p1.x - p0.x) +
        b * (p2.x - p1.x) +
        c * (p3.x - p2.x),

        a * (p1.y - p0.y) +
        b * (p2.y - p1.y) +
        c * (p3.y - p2.y)
    );
}

// creates a Bezier curve from the supplied control points
Bezier::Bezier(const std::vector<Point2D>& pts)
    : mPoints(pts)
{
    CheckPoints(mPoints);
}

// returns a point on the stored curve
Point2D Bezier::GetPoint(float t) const
{
    return GetPoint(mPoints, t);
}

// returns the slope of the stored curve
Point2D Bezier::GetSlope(float t) const
{
    return GetSlope(mPoints, t);
}

// returns point on a curve containing one or more segments
Point2D Bezier::GetPoint(
    const std::vector<Point2D>& pts, float t)
{
    CheckPoints(pts);

    size_t segmentCount = (pts.size() - 1) / 3;

    // keep t within the valid range of curve
    if (t < 0.0f) {
        t = 0.0f;
    }

    if (t > static_cast<float>(segmentCount)) {
        t = static_cast<float>(segmentCount);
    }

    // find the segment containing t
    size_t segment = static_cast<size_t>(t);

    // deal w the final endpoint separately
    if (segment == segmentCount) {
        segment = segmentCount - 1;
    }

    // convert global t into a parameter from 0-1
    float localT = t - static_cast<float>(segment);

    return CalculatePoint(pts, segment * 3, localT);
}

// returns slope on curve containing one/more segments
Point2D Bezier::GetSlope(
    const std::vector<Point2D>& pts, float t)
{
    CheckPoints(pts);

    size_t segmentCount = (pts.size() - 1) / 3;

    if (t < 0.0f) {
        t = 0.0f;
    }

    if (t > static_cast<float>(segmentCount)) {
        t = static_cast<float>(segmentCount);
    }

    size_t segment = static_cast<size_t>(t);

    if (segment == segmentCount) {
        segment = segmentCount - 1;
    }

    float localT = t - static_cast<float>(segment);

    return CalculateSlope(pts, segment * 3, localT);
}

} // namespace CMPUT350
