#include "GameEngine.h"

#include <algorithm>

#include "CollisionObject.h"
#include "DrawContext.h"
#include "GameContext.h"
#include "GraphicsObject.h"
#include "NotificationManager.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"

/**
 * @brief Finds where a directed line first meets a rectangle
 *
 * The rectangle is treated as four side lines. Every side the line crosses is a hit, and the
 * hit nearest the line's start (p1) is kept, since that is the first thing the line meets.
 * A line lying entirely inside the rectangle crosses no side and is reported as no hit.
 *
 * @param line The directed line; p1 is where it starts
 * @param rect The axis-aligned rectangle to test against
 * @param hitPoint Set to the first hit point when the result is true; unchanged otherwise
 * @return True if the line crosses at least one side of the rectangle
 */
static bool LineHitsRect(const Line& line, const Rect& rect, Point2D& hitPoint) {
    // Corners of the rect; screen y grows downward, so the bottom edge is topLeft.y + height
    Point2D topLeft = rect.topLeft;
    Point2D topRight = Point2D(rect.topLeft.x + rect.width, rect.topLeft.y);
    Point2D bottomLeft = Point2D(rect.topLeft.x, rect.topLeft.y + rect.height);
    Point2D bottomRight = Point2D(topRight.x, topRight.y + rect.height);

    // The rect's outline as four lines: top, right, bottom, left
    Line sides[4] = {
        Line(topLeft, topRight),  // top
        Line(topRight, bottomRight),
        Line(bottomLeft, bottomRight),
        Line(topLeft, bottomLeft),

    };

    // found: any side crossed yet; bestDistance: distance from line.p1 to the nearest hit so far
    bool found = false;
    float bestDistance = 0;

    for (int i = 0; i < 4; i++) {
        Point2D crossingPoint;

        if (line.Crosses(sides[i], crossingPoint)) {
            float d = line.p1.Distance(crossingPoint);
            // Keep this hit if it is the first one, or nearer to the line's start than the best so
            // far
            if (!found || d < bestDistance) {
                found = true;
                bestDistance = d;
                hitPoint = crossingPoint;
            }
        }
    }
    return found;
}

static bool CircleHitsRect(const Circle& circle, const Rect& rect) {
    float nearestX = std::clamp(circle.center.x, rect.topLeft.x, rect.topLeft.x + rect.width);
    float nearestY = std::clamp(circle.center.y, rect.topLeft.y, rect.topLeft.y + rect.height);

    float d = circle.center.Distance(Point2D(nearestX, nearestY));
    return (d < circle.radius);
}

/**
 * @brief Constructs a game engine with a window of the specified size and name.
 *
 * @param width Width of the game window in pixels.
 * @param height Height of the game window in pixels.
 * @param name Title displayed on the game window.
 */
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    mWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), name);
    mWindow->setFramerateLimit(30);

    mFont = std::make_shared<sf::Font>();

    // Sample font loading code
    if (!mFont->openFromMemory(&_font, _font_len)) {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }
}

/**
 * @brief Destroys the game engine and closes the game window.
 *
 * The engine does not return a value.
 */
GameEngine::~GameEngine() {
    // Cleanup resources
    mWindow->close();
}

/**
 * @brief Adds a game object to the list of objects waiting to be activated.
 *
 * Objects are placed in mAddedGameObjects instead of immediately being added
 * to mGameObjects. This prevents objects from being added to the active object
 * list in the middle of an engine update.
 *
 * @param gameObject Shared pointer to the game object that should be added.
 *
 * The function does not return a value.
 */
void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    mAddedGameObjects.push_back(gameObject);
}

/**
 * @brief Registers a notification for a key-press event.
 *
 * @param key The keyboard scancode.
 * @param notification The notification message to send.
 */
void GameEngine::InstallKeyDownNotification(int key, const std::string& notification) {
    mKeyDownNotifications[key].push_back(notification);
}

/**
 * @brief Registers a notification for a key-release event.
 *
 * @param key The keyboard scancode.
 * @param notification The notification message to send.
 */
void GameEngine::InstallKeyUpNotification(int key, const std::string& notification) {
    mKeyUpNotifications[key].push_back(notification);
}

/**
 * @brief Returns an iterator to the first active game object.
 */
EngineView::const_iterator GameEngine::begin() const { return mGameObjects.begin(); }

/**
 * @brief Returns an iterator past the last active game object.
 */
EngineView::const_iterator GameEngine::end() const { return mGameObjects.end(); }

/**
 * @brief Returns a const iterator to the first active game object.
 */
EngineView::const_iterator GameEngine::cbegin() const { return mGameObjects.cbegin(); }

/**
 * @brief Returns a const iterator past the last active game object.
 */
EngineView::const_iterator GameEngine::cend() const { return mGameObjects.cend(); }

/**
 * @method Run
 * @arguments None
 * @description Gives control to the game engine. Will not return until the game window is closed or
 * all objects have been destroyed.
 *
 * @brief Runs the main game loop until the game window is closed.
 *
 * The engine processes objects in the following order each frame:
 * 0. Remove dead objects.
 * 1. Activate and initialize newly added objects.
 * 2. Process keyboard and window events.
 * 3. Update active game objects.
 * 4. Check collisions between collision objects.
 * 5. Run late updates.
 * 6. Render background graphics.
 * 7. Render foreground graphics.
 *
 * The function does not return a value.
 */
void GameEngine::Run() {
    int frame = 0;
    while (mWindow->isOpen())  // window is open
    {
        // Create a context for this frame so game objects can access the
        // engine and drawing system without directly owning either one.
        GameContext context;
        context.mEngineView = this;  // pointer to current GameEngine obj
        context.mNotificationManager = &mNotificationManager;
        context.currentFrame = frame;
        frame += 1;
        DrawContext screenContext(mWindow, mFont);
        context.ScreenContext = &screenContext;

        DrawContext guiContext(mWindow, mFont);
        context.GUIContext = &guiContext;

        // 0. Remove any objects that are now dead
        // mGameObjects = vector containing all game elements active rn
        auto it = mGameObjects.begin();  // .begin gives iterator pointing to first object
        while (it !=
               mGameObjects.end()) {  // .end = position just after last element (not actual object)
            if (!(*it)->IsAlive()) {  // if actual element during the iteration is returned true
                                      // alive
                it = mGameObjects.erase(it);  // iterator pointed to erased object no longer valid
            } else {
                ++it;  // if object still alive, move on
            }
        }

        // 1. Activate and initialize any objects added during the last frame
        for (auto gameObject :
             mAddedGameObjects) {                // goes through every object in mAddedGameObjects
            mGameObjects.push_back(gameObject);  // adds obj to end of mGameObjects vector
            context.CurrObject = gameObject;     //
            gameObject->Initialize(&context);
        }
        mAddedGameObjects.clear();

        // 2. Process events
        while (const std::optional event =
                   mWindow->pollEvent()) {         // checks all events done by player
            if (event->is<sf::Event::Closed>()) {  // checks if event was closing the window
                mWindow->close();
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                int key = static_cast<int>(keyPressed->scancode);

                auto it = mKeyDownNotifications.find(key);
                if (it != mKeyDownNotifications.end()) {
                    for (const std::string& notification : it->second) {
                        mNotificationManager.Notify(notification);
                    }
                }
            } else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                int key = static_cast<int>(keyReleased->scancode);

                auto it = mKeyUpNotifications.find(key);
                if (it != mKeyUpNotifications.end()) {
                    for (const std::string& notification : it->second) {
                        mNotificationManager.Notify(notification);
                    }
                }
            }
        }

        // 3. Update game objects
        // Each active object gets one opportunity to update its state for this frame.
        for (auto gameObject : mGameObjects) {
            gameObject->Update(&context);  // updates every active game object's function
        }

        // 4. Process collision events
        // Only CollisionObject instances participate in collision detection.
        // The nested loop starts b at a + 1 so each pair is checked exactly once
        // and an object is never checked against itself.
        for (size_t a = 0; a < mGameObjects.size(); ++a) {  // goes through every obj
            auto objA = std::dynamic_pointer_cast<CollisionObject>(
                mGameObjects[a]);  // checks if game obj is also collission obj
            if (objA == nullptr)   // if not, skip
            {
                continue;
            }

            for (size_t b = a + 1; b < mGameObjects.size();
                 ++b) {  // checking objects after a to avoid checking same pair
                auto objB = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[b]);
                if (objB == nullptr) {
                    continue;
                }
                // with help from OpenAI: https://chatgpt.com/s/t_6abdcb6da1f08191a932fead94213877
                // with prompt: implementation of if 2 rectangles make a positive overlapped area
                // with rectangles, how to detect that get each obj's collission rectangle bounds
                // using copies to not modify rectangle that will happen
                Rect boundsA = objA->GetBounds();
                Rect boundsB = objB->GetBounds();

                // replace boundsA with area where A and B overlap
                boundsA &= boundsB;

                if (boundsA.width > 0 &&
                    boundsA.height > 0) {  // if 2 rectangles made positive overlapped area

                    context.ScreenContext->SetContextOffset(objA->GetLocation());
                    context.ScreenContext->SetContextRotation(objA->GetRotation());

                    const std::vector<Shape>& shapeA = objA->GetShapes();
                    std::vector<Shape> screenA;

                    for (const Shape& s : shapeA) {
                        if (s.t == ShapeType::kLine) {
                            Point2D p1 = context.ScreenContext->Transform(s.shape.line.p1);
                            Point2D p2 = context.ScreenContext->Transform(s.shape.line.p2);
                            screenA.push_back(CMPUT350::Line(p1, p2));
                        } else if (s.t == ShapeType::kRect) {
                            Rect box = context.ScreenContext->Transform(s.shape.rect);
                            screenA.push_back(box);
                        } else if (s.t == ShapeType::kCircle) {
                            Point2D newCenter =
                                context.ScreenContext->Transform(s.shape.circle.center);
                            screenA.push_back(CMPUT350::Circle(newCenter, s.shape.circle.radius));
                        }
                    }

                    context.ScreenContext->SetContextOffset(objB->GetLocation());
                    context.ScreenContext->SetContextRotation(objB->GetRotation());

                    const std::vector<Shape>& shapeB = objB->GetShapes();
                    std::vector<Shape> screenB;

                    for (const Shape& s : shapeB) {
                        if (s.t == ShapeType::kLine) {
                            Point2D p1 = context.ScreenContext->Transform(s.shape.line.p1);
                            Point2D p2 = context.ScreenContext->Transform(s.shape.line.p2);
                            screenB.push_back(CMPUT350::Line(p1, p2));
                        } else if (s.t == ShapeType::kRect) {
                            Rect box = context.ScreenContext->Transform(s.shape.rect);
                            screenB.push_back(box);
                        } else if (s.t == ShapeType::kCircle) {
                            Point2D newCenter =
                                context.ScreenContext->Transform(s.shape.circle.center);
                            screenB.push_back(CMPUT350::Circle(newCenter, s.shape.circle.radius));
                        }
                    }

                    // collided: a line crossed a rect
                    bool collided = false;
                    // hitPoint: where the line first met the rect, in screen space
                    Point2D hitPoint;
                    // lineIsA: true if objA owned the line, false if objB did
                    bool lineIsA = false;
                    // shapesTouch: two rects or two circles overlap; there is no single point for
                    // that
                    bool shapesTouch = false;
                    for (const Shape& sa : screenA) {
                        for (const Shape& sb : screenB) {
                            if (sa.t == ShapeType::kRect && sb.t == ShapeType::kLine) {
                                if (LineHitsRect(sb.shape.line, sa.shape.rect, hitPoint)) {
                                    collided = true;
                                    lineIsA = false;
                                }
                            } else if (sa.t == ShapeType::kLine && sb.t == ShapeType::kRect) {
                                if (LineHitsRect(sa.shape.line, sb.shape.rect, hitPoint)) {
                                    collided = true;
                                    lineIsA = true;
                                }
                            } else if (sa.t == ShapeType::kRect && sb.t == ShapeType::kRect) {
                                // Overlap of two rects is an area, so this is only a yes/no and
                                // gives no hit point. Copies, because &= changes the left side.
                                Rect rectA = sa.shape.rect;
                                Rect rectB = sb.shape.rect;
                                rectA &= rectB;
                                if (rectA.width > 0 && rectA.height > 0) {
                                    shapesTouch = true;
                                }
                            } else if (sa.t == ShapeType::kCircle && sb.t == ShapeType::kCircle) {
                                float d = sa.shape.circle.center.Distance(sb.shape.circle.center);
                                if (d < sa.shape.circle.radius + sb.shape.circle.radius) {
                                    shapesTouch = true;
                                }
                            } else if (sa.t == ShapeType::kCircle && sb.t == ShapeType::kRect) {
                                if (CircleHitsRect(sa.shape.circle, sb.shape.rect)) {
                                    shapesTouch = true;
                                }

                            } else if (sa.t == ShapeType::kRect && sb.t == ShapeType::kCircle) {
                                if (CircleHitsRect(sb.shape.circle, sa.shape.rect)) {
                                    shapesTouch = true;
                                }
                            } else if (sa.t == ShapeType::kLine && sb.t == ShapeType::kLine) {
                                Point2D crossingPoint;
                                if (sa.shape.line.Crosses(sb.shape.line, crossingPoint)) {
                                    shapesTouch = true;
                                }
                            }
                        }
                    }

                    // Line hit: only the line's owner gets the hit point, in its own local
                    // coordinates; the other object gets the plain message. A line hit takes
                    // priority over other shapes touching(the else if below), so a pair is not
                    // notified twice.
                    if (collided) {
                        // Point the context at the line's owner so ReverseTransform gives that
                        // object's local numbers. Otherwise it is still pointed at objB.
                        if (lineIsA) {
                            context.ScreenContext->SetContextOffset(objA->GetLocation());
                            context.ScreenContext->SetContextRotation(objA->GetRotation());
                        }

                        Point2D localHit = context.ScreenContext->ReverseTransform(hitPoint);

                        if (lineIsA) {
                            objA->CollisionEnter(objB, localHit);
                            objB->CollisionEnter(objA);
                        } else {
                            objB->CollisionEnter(objA, localHit);
                            objA->CollisionEnter(objB);
                        }
                    } else if (shapesTouch) {
                        // Rect vs rect or circle vs circle: no single hit point, so both get the
                        // plain message.
                        objA->CollisionEnter(objB);
                        objB->CollisionEnter(objA);
                    }
                }
            }
        }

        // 5. Late updates
        // LateUpdate runs after all regular updates and collision processing.
        // This allows objects to perform actions that depend on the results
        // of the current frame's updates and collisions.
        for (auto gameObject : mGameObjects) {
            gameObject->LateUpdate(&context);  // calls each object's latest update
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        // Background graphics are drawn first so that foreground objects
        // appear on top of them.
        for (auto gameObject : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(
                gameObject);  // checks if gameobj also graphicsobj

            if (graphicsObject != nullptr) {  // if so, call render func
                // Move/rotate the draw context to this object, so it draws in its local coordinates
                context.ScreenContext->SetContextOffset(graphicsObject->GetLocation());
                context.ScreenContext->SetContextRotation(graphicsObject->GetRotation());
                graphicsObject->RenderBackground(&context);
            }
        }

        // 7. Render foreground
        // Foreground graphics are drawn after the background so game objects
        // such as the player, enemies, and bullets appear on top.
        for (auto gameObject : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);

            if (graphicsObject != nullptr) {
                // Same per-object transform as the background pass above
                context.ScreenContext->SetContextOffset(graphicsObject->GetLocation());
                context.ScreenContext->SetContextRotation(graphicsObject->GetRotation());
                graphicsObject->RenderForeground(&context);
            }
        }

        for (auto gameObject : mGameObjects) {
            gameObject->RenderUI(&context);
        }
        // Actually render to window
        mWindow->display();
    }
}

// Sample code for processing events

// bool GameEngine::ProcessEvents(GameContext *context)
//{
//	while (const std::optional event = mWindow->pollEvent())
//	{
//		if (event->is<sf::Event::Closed>())
//		{
//		}
//		else if (event->is<sf::Event::Resized>())
//		{
//		}
//		else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>())
//		{
//			// use keyPressed->unicode to get character
//		}
//	}
// }

}  // namespace CMPUT350
