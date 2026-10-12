#include "DrawContext.h"
#include <cmath>

namespace CMPUT350 {

/**
 * @brief Constructs a DrawContext bound to a window and a font
 *
 * @param window The render window all draw calls will target
 * @param font The font used by DrawText and DrawCenteredText
 */
DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}

/**
 * @brief Draws text centered on a given point
 *
 * @param text The string to draw
 * @param pixelSize The character size, in pixels
 * @param p The point the text is centered on
 * @param c The fill color of the text
 */
void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textShapeCentered(*mFont);
    textShapeCentered.setString(text);
    textShapeCentered.setCharacterSize(pixelSize);
    textShapeCentered.setFillColor(sf::Color(c.r, c.g,  c.b));
    sf::FloatRect bounds = textShapeCentered.getLocalBounds(); // Rendered text's own local box (position + size), needed to compute its center
    textShapeCentered.setOrigin(bounds.position + bounds.size / 2.f); // Anchor = center of text's box, so setPosition below centers text instead of anchoring its corner
    Point2D transformedPos = Transform(p);
    textShapeCentered.setPosition({transformedPos.x, transformedPos.y});
    mWindow->draw(textShapeCentered);
}

/**
 * @brief Draws text with its top-left corner at a given point
 *
 * @param text The string to draw
 * @param pixelSize The character size, in pixels
 * @param p The top-left position of the text
 * @param c The fill color of the text
 */
void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textShape(*mFont);
    textShape.setString(text);
    textShape.setCharacterSize(pixelSize);
    textShape.setFillColor(sf::Color(c.r, c.g,  c.b));
    Point2D transformedPos = Transform(p);
    textShape.setPosition({transformedPos.x, transformedPos.y});
    mWindow->draw(textShape);
}

/**
 * @brief Draws a filled circle centered on a given point
 *
 * @param p The center of the circle
 * @param radius The radius of the circle, in pixels
 * @param c The fill color of the circle
 */
void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius}); // Moves anchor dot goes at the exact middle of this circle's own private box
    Point2D transformedPos = Transform(p);
    shape.setPosition({transformedPos.x, transformedPos.y}); // Move circle to the screen position of p
    shape.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(shape);
}   

/**
 * @brief Draws a filled rectangle
 *
 * @param r The rectangle's position and size
 * @param c The fill color of the rectangle
 */
void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape shape({r.width, r.height});
    Point2D transformedTopLeft = Transform(r.topLeft);
    shape.setPosition({transformedTopLeft.x, transformedTopLeft.y});
    shape.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(shape);
}

/**
 * @brief Draws an unfilled rectangle outline
 *
 * @param r The rectangle's position and size
 * @param width The outline thickness, in pixels
 * @param c The color of the outline
 */
void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    sf::RectangleShape shape({r.width, r.height});
    Point2D transformedTopLeft = Transform(r.topLeft);
    shape.setPosition({transformedTopLeft.x, transformedTopLeft.y});
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color(c.r, c.g, c.b));
    shape.setOutlineThickness(width);
    mWindow->draw(shape);
}

/**
 * @brief Draws a line between two points with a specified width and color.
 *
 * @param from The starting point of the line (Point2D).
 * @param to The ending point of the line (Point2D).
 * @param width The width of the line in pixels.
 * @param c The color of the line, specified as an RGBColor object.
 *
 * This function calculates the distance and angle between the two points
 * and uses a polygone shape to represent the line. The line is drawn
 * relative to the world offset and rendered onto the associated window.
 */

 /**
 * AI disclosure (DrawLine):
 * Used Claude to work through "how to draw a line with width" since SFML has no
 * built-in shape for that. AI-suggested pieces:
 *   - Model the line as a thin sf::ConvexShape rectangle, not a real line primitive
 *   - Perpendicular vector via the (-y, x) swap-and-negate rotation trick
 *   - Corner formulas: p0=from+offset, p1=to+offset, p2=to-offset, p3=from-offset
 *    
 * Prompts (paraphrased, extended back-and-forth): "how do we draw a line with
 * width in SFML", "why normalize direction first", "why offset by half not full
 * width", "why perpendicular specifically", "how do we know which corner is +/-
 * offset", "whyConvexShape"
 * Reflection: Going back and forth with AI conversation didn't relly help me understand, so I asked
 * them to generate visual like this to help me understand whats going on better: https://claude.ai/artifact/KMb8Lb9FL4Przf9WhJHJqH
 * 
 */
void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c) {
    Point2D transformedFromPos = Transform(from);
    Point2D transformedFromTo = Transform(to);
    Point2D direction = transformedFromTo - transformedFromPos;
    direction.Normalize(); 
    Point2D perpendicular(-direction.y, direction.x); 
    Point2D offset = perpendicular * (width / 2.0f);

    Point2D p0 = transformedFromPos + offset;
    Point2D p1 = transformedFromTo + offset; 
    Point2D p2 = transformedFromTo - offset; 
    Point2D p3 = transformedFromPos - offset; 

    sf::ConvexShape shape;
    shape.setPointCount(4);
    shape.setPoint(0, {p0.x, p0. y});
    shape.setPoint(1, {p1.x, p1. y});
    shape.setPoint(2, {p2.x, p2. y});
    shape.setPoint(3, {p3.x, p3. y});

    shape.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(shape);

}

/**
 * @brief Returns the window's current width, in pixels
 */
int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

/**
 * @brief Returns the window's current height, in pixels
 */
int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }


/**
 * @brief Sets the screen position that an object's local origin (0, 0) maps to
 *
 * Called by the engine before an object renders, with that object's GetLocation().
 *
 * @param p Screen position of the object's location, in pixels
 */
void DrawContext::SetContextOffset(Point2D p) {
    mOffset = p;
}

/**
 * @brief Sets the rotation applied to everything drawn for the current object
 *
 * Called by the engine before an object renders, with that object's GetRotation().
 *
 * @param rotation Angle in radians, applied about the object's local origin
 */
void DrawContext::SetContextRotation(float rotation) {
    mRotation = rotation;
}

/**
 * @brief Converts a point from the object's local space to screen space
 *
 * Rotates the point about the local origin by mRotation, then shifts it by mOffset.
 * Every Draw call runs its points through this, which is why objects draw around (0, 0).
 *
 * @param p Point in the object's local coordinates
 * @return The same point in screen coordinates, in pixels
 */
Point2D DrawContext::Transform(Point2D p) {
    float cosine = std::cos(mRotation);
    float sine = std::sin(mRotation);

    float newX = p.x * cosine - p.y * sine; 
    float newY = p.x * sine + p.y * cosine;

    Point2D rotated(newX, newY);
    return rotated + mOffset;
}

/**
 * @brief Converts a point from screen space back to the object's local space
 *
 * Undoes Transform: subtracts mOffset first, then rotates by -mRotation. The order matters
 * because Transform rotated first and shifted second.
 *
 * @param p Point in screen coordinates, in pixels
 * @return The same point in the object's local coordinates
 */
Point2D DrawContext::ReverseTransform(Point2D p) {
    Point2D shifted(p - mOffset);
    float cosine = std::cos(-mRotation);
    float sine = std::sin(-mRotation);

    float newX = shifted.x * cosine - shifted.y * sine; 
    float newY = shifted.x * sine + shifted.y * cosine;

    return Point2D (newX, newY);

}

}  // namespace CMPUT350
