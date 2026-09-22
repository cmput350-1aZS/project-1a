#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <algorithm>
#include <cmath>
#include <iostream>

namespace CMPUT350 {

struct Point2D {
    float x, y;
    // Constructs a point, defaulting to the origin (0, 0)
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}
    // Returns the Euclidean distance to another point
    double Distance(const Point2D& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
    // Adds two points component-wise
    Point2D operator+(const Point2D& other) const { return Point2D(x + other.x, y + other.y); }
    // Adds a scalar to both components
    Point2D operator+(const float& other) const { return Point2D(x + other, y + other); }
    // Subtracts another point component-wise
    Point2D operator-(const Point2D& other) const { return Point2D(x - other.x, y - other.y); }
    // Subtracts a scalar from both components
    Point2D operator-(const float& other) const { return Point2D(x - other, y - other); }
    // Scales both components by a scalar
    Point2D operator*(const float& scalar) const { return Point2D(x * scalar, y * scalar); }
    // Adds a scalar to both components in place (modifies object itself, not returning new one)
    Point2D& operator+=(const float& scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }
    // Adds another point to this one in place
    Point2D& operator+=(const Point2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    // Subtracts another point from this one in place
    Point2D& operator-=(const Point2D& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    // Checks exact equality of both components
    bool operator==(const Point2D& other) const {
        if (x == other.x && y == other.y) {
            return true;
        } else {
            return false;
        }
    }
    // Scales both components by an integer in place
    Point2D& operator*=(const int& scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    // Divides both components by an integer in place
    Point2D& operator/=(const int& scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    // Dot product operator
    float operator*(const Point2D& other) const { return x * other.x + y * other.y; }
    // Returns the dot product with another point
    float Dot(Point2D b) const { return x * b.x + y * b.y; }
    // Return the 2D cross product with another point
    float Cross(Point2D b) const { return x * b.y - y * b.x; }
    // Returns the dot product of two points
    static float Dot(Point2D a, Point2D b) { return a.x * b.x + a.y * b.y; }
    // Returns the 2D cross product of two points
    static float Cross(Point2D a, Point2D b) { return a.x * b.y - a.y * b.x; }
    // Scales this vector to unit length, does nothing if the length is zero
    void Normalize() {
        float length = std::sqrt(x * x + y * y);

        if (length != 0.0f) {
            x /= length;
            y /= length;
        }
    }
};

// Prints a point as (x, y)
static std::ostream& operator<<(std::ostream& os, const Point2D& p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

// Scales a point by a scalar, with the scalar on the left-hand side
static Point2D operator*(float number, const Point2D& rhs) {
    float scaledX = number * rhs.x;
    float scaledY = number * rhs.y;
    return Point2D(scaledX, scaledY);
}

struct Line {
    Point2D p1, p2;

    // Constructs a segment, defaulting to a zero-length segment at the origin
    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}
    // Constructs a segment from raw endpoint coordinates
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    // Returns the length of the segment
    float Length() const { return p1.Distance(p2); }
    // Returns the point on this segment closest to p
    Point2D ClosestPoint(const Point2D& p) const {
        if (p1 == p2) {
            return p1;
        }
        // Vector pointing from the segment's start point (p1) toward its end point (p2)
        Point2D direction = p2 - p1;
        // Vector pointing from the segment's start point (p1) toward the input point (p)
        Point2D toPoint = p - p1;

        // Determines how far to move from p1 toward p2 to reach the closest point to p
        // fractionAlongSegment = 0 means p1, fractionAlongSegment = 1 means p2
        float fractionAlongSegment =
            Point2D::Dot(toPoint, direction) / Point2D::Dot(direction, direction);

        // fractionAlongSegment may be outside [0, 1], so it must be clamped for a finite segment
        if (fractionAlongSegment < 0.0f) {
            fractionAlongSegment = 0.0f;
        } else if (fractionAlongSegment > 1.0f) {
            fractionAlongSegment = 1.0f;
        }

        // Walk fractionAlongSegment of the way from p1 toward p2 to get the closest point
        return p1 + direction * fractionAlongSegment;
    }
    // Finds where this segment crosses another segment.
    // Returns true and sets crossingPoint if the segments intersect within both of their bounds,
    // returns false (leaving crossingPoint unset) if they are parallel/collinear or don't overlap.
    bool Crosses(Line other, Point2D& crossingPoint) const {
        Point2D thisDirection = p2 - p1;
        Point2D otherDirection = other.p2 - other.p1;
        Point2D diff = other.p1 - p1;  // the offset between line 1 and where line 2 starts

        // Cross product of the two direction vectors; zero means the segments are parallel
        // so there is no single crossing point
        float denom = Point2D::Cross(thisDirection, otherDirection);

        if (denom == 0.0f) {
            return false;
        }

        float t = Point2D::Cross(diff, otherDirection) / denom;  // fraction along this segment
        float u = Point2D::Cross(diff, thisDirection) / denom;   // fraction along the other segment

        if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
            crossingPoint = p1 + thisDirection * t;
            return true;
        }
        return false;
    }
};

// Prints a segment as p1->p2
static std::ostream& operator<<(std::ostream& os, const Line& l) {
    os << l.p1 << "->" << l.p2;
    return os;
}

// A circle defined by a center point and radius
struct Circle {
    Point2D center;
    float radius;

    // Constructs a circle from a center point and radius, defaulting to a zero circle at the origin
    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    // Constructs a circle from raw center coordinates and radius
    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};


struct Rect {
    Point2D topLeft;
    float width, height;

    // Constructs a rect from raw left/top/width/height values
    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(left, top)), width(width), height(height) {}

    // Constructs a rect from a top-left point and size, defaulting to a zero rect at the origin
    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    // Constructs a square bounding box centered on a point, extending radius in each direction
    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    // Expands this rect to be the union (bounding box) of this rect and another
    Rect& operator|=(const Rect& other) {
        float thisLeft = topLeft.x;             // x-coordinate of the left edge
        float thisTop = topLeft.y;              // y-coordintae of the top edge
        float thisRight = topLeft.x + width;    // x-coordinate of right edge
        float thisBottom = topLeft.y + height;  // y-coordinate of bottom edge

        float otherLeft = other.topLeft.x;
        float otherTop = other.topLeft.y;
        float otherRight = other.topLeft.x + other.width;
        float otherBottom = other.topLeft.y + other.height;

        float newLeft = std::min(thisLeft, otherLeft);
        float newTop = std::min(
            thisTop, otherTop);  // top edge = smallest y, since y increases downward on screen
        float newRight = std::max(thisRight, otherRight);
        float newBottom = std::max(thisBottom, otherBottom);

        topLeft = Point2D(newLeft, newTop);
        width = newRight - newLeft;
        height = newBottom - newTop;

        return *this;
    }
    // Expands this rect to include the given point
    Rect& operator|=(const Point2D& other) {
        float thisLeft = topLeft.x;
        float thisTop = topLeft.y;
        float thisRight = topLeft.x + width;
        float thisBottom = topLeft.y + height;

        float newLeft = std::min(thisLeft, other.x);
        float newTop = std::min(thisTop, other.y);
        float newRight = std::max(thisRight, other.x);
        float newBottom = std::max(thisBottom, other.y);

        topLeft = Point2D(newLeft, newTop);
        width = newRight - newLeft;
        height = newBottom - newTop;

        return *this;
    }
    // Expands this rect to include both endpoints of the given line
    Rect& operator|=(const Line& other) {
        *this |= other.p1;
        *this |= other.p2;
        return *this;
    }
    // Shrinks this rect to the overlapping area with another rect;
    // if they don't overlap, becomes zero-sized but keeps this rect's original top-left corner
    Rect& operator&=(const Rect& other) {
        float thisLeft = topLeft.x;
        float thisTop = topLeft.y;
        float thisRight = topLeft.x + width;
        float thisBottom = topLeft.y + height;

        float otherLeft = other.topLeft.x;
        float otherTop = other.topLeft.y;
        float otherRight = other.topLeft.x + other.width;
        float otherBottom = other.topLeft.y + other.height;

        float newLeft = std::max(thisLeft, otherLeft);
        float newTop = std::max(thisTop, otherTop);
        float newRight = std::min(thisRight, otherRight);
        float newBottom = std::min(thisBottom, otherBottom);

        // There is no overlap
        if (newLeft > newRight || newTop > newBottom) {
            width = 0;
            height = 0;
        } else {
            topLeft = Point2D(newLeft, newTop);
            width = newRight - newLeft;
            height = newBottom - newTop;
        }

        return *this;
    }
    // Translates this rect in place by the given offset
    Rect& operator+=(const Point2D& other) {
        topLeft = topLeft + other;
        return *this;
    }
    // Returns a copy of this rect translated by the given offset
    Rect operator+(const Point2D& other) const {
        Rect copy = *this;
        copy += other;
        return copy;
    }
    // Shrinks the rect on all sides by inset 
    void Inset(int inset) {
        topLeft.x += inset;
        topLeft.y += inset;
        width = width - (2 * inset);
        height = height - (2 * inset);
    }
    // Returns true if the given point lies within this rect, inclusive of its edges
    bool IsInside(const Point2D& p) const {
        float left = topLeft.x;
        float top = topLeft.y;
        float right = topLeft.x + width;
        float bottom = topLeft.y + height;

        if (left <= p.x && p.x <= right && top <= p.y && p.y <= bottom) {
            return true;
        }
        return false;
    }
};

// Prints a rect as its top-left point, width, and height
static std::ostream& operator<<(std::ostream& os, const Rect& l) {
    os << l.topLeft << " width=" << l.width << " height=" << l.height;
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
