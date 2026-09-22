#include "DrawContext.h"

namespace CMPUT350 {

DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}

void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textShapeCentered(*mFont);
    textShapeCentered.setString(text);
    textShapeCentered.setCharacterSize(pixelSize);
    textShapeCentered.setFillColor(sf::Color(c.r, c.g,  c.b));
    sf::FloatRect bounds = textShapeCentered.getLocalBounds(); // Rendered text's own local box (position + size), needed to compute its center
    textShapeCentered.setOrigin(bounds.position + bounds.size / 2.f); // Anchor = center of text's box, so setPosition below centers text instead of anchoring its corner
    textShapeCentered.setPosition({p.x, p.y});
    mWindow->draw(textShapeCentered);
}

void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c) {
    sf::Text textShape(*mFont);
    textShape.setString(text);
    textShape.setCharacterSize(pixelSize);
    textShape.setFillColor(sf::Color(c.r, c.g,  c.b));
    textShape.setPosition({p.x, p.y});
    mWindow->draw(textShape);
}

void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c) {
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius}); // Moves anchor dot goes at the exact middle of this circle's own private box
    shape.setPosition({p.x, p.y});     // Move circle to position p
    shape.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(shape);
}   

void DrawContext::DrawRect(Rect r, RGBColor c) {
    sf::RectangleShape shape({r.width, r.height});
    shape.setPosition({r.topLeft.x, r.topLeft.y});
    shape.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(shape);
}

void DrawContext::FrameRect(Rect r, float width, RGBColor c) {
    sf::RectangleShape shape({r.width, r.height});
    shape.setPosition({r.topLeft.x, r.topLeft.y});
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
    Point2D direction = to - from;
    direction.Normalize(); 
    Point2D perpendicular(-direction.y, direction.x); 
    Point2D offset = perpendicular * (width / 2.0f);

    Point2D p0 = from + offset;
    Point2D p1 = to + offset; 
    Point2D p2 = to - offset; 
    Point2D p3 = from - offset; 

    sf::ConvexShape shape;
    shape.setPointCount(4);
    shape.setPoint(0, {p0.x, p0. y});
    shape.setPoint(1, {p1.x, p1. y});
    shape.setPoint(2, {p2.x, p2. y});
    shape.setPoint(3, {p3.x, p3. y});

    shape.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(shape);

}

int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350
