#include "GameEngine.h"
#include "GameContext.h"
#include "DrawContext.h"
#include "CollisionObject.h"
#include "GraphicsObject.h"

/// @brief
namespace CMPUT350 {
#include "FontData.h"

/**
 * @brief Constructs a game engine with a window of the specified size and name.
 *
 * @param width Width of the game window in pixels.
 * @param height Height of the game window in pixels.
 * @param name Title displayed on the game window.
 */
GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name) {
    mWindow = std::make_shared<sf::RenderWindow>(
        sf::VideoMode({width, height}),
        name
    );
    mWindow->setFramerateLimit(30);

    mFont = std::make_shared<sf::Font>();

    // Sample font loading code
    if (!mFont->openFromMemory(&_font, _font_len))
    {
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
    while (mWindow->isOpen())  // window is open
    {
        // Create a context for this frame so game objects can access the
        // engine and drawing system without directly owning either one.
        GameContext context;
        context.mEngineView = this; // pointer to current GameEngine obj
        DrawContext screenContext(mWindow, mFont);
        context.ScreenContext = &screenContext;

        // 0. Remove any objects that are now dead
        // mGameObjects = vector containing all game elements active rn
        auto it = mGameObjects.begin(); // .begin gives iterator pointing to first object
        while (it != mGameObjects.end()) { // .end = position just after last element (not actual object)
            if (!(*it)->IsAlive()) { // if actual element during the iteration is returned true alive
                it = mGameObjects.erase(it); // iterator pointed to erased object no longer valid
            }
            else {
                ++it; // if object still alive, move on
            }
        }

        // 1. Activate and initialize any objects added during the last frame
        for (auto gameObject : mAddedGameObjects) { // goes through every object in mAddedGameObjects
            mGameObjects.push_back(gameObject); // adds obj to end of mGameObjects vector

            gameObject->Initialize(&context);
        }
        mAddedGameObjects.clear();

        // 2. Process events
        while (const std::optional event = mWindow->pollEvent()) { // checks all events done by player
            if (event->is<sf::Event::Closed>()) { // checks if event was closing the window
                mWindow->close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::TextEntered>()) { // checks if event done was a keyboard input (textentered)
                // Every active object receives the keyboard event so objects can
                // decide whether they need to respond to the key.
                for (auto gameObject : mGameObjects) { // every object gets to react to event done by player
                    gameObject->HandleKeyEvent(&context, static_cast<char>(keyPressed->unicode));
                }
            }
        }

        // 3. Update game objects
        // Each active object gets one opportunity to update its state for this frame.
        for (auto gameObject : mGameObjects) {
            gameObject->Update(&context); // updates every active game object's function
        }

        // 4. Process collision events
        // Only CollisionObject instances participate in collision detection.
        // The nested loop starts b at a + 1 so each pair is checked exactly once
        // and an object is never checked against itself.
        for (size_t a = 0; a < mGameObjects.size(); ++a) { // goes through every obj
            auto objA = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[a]); // checks if game obj is also collission obj
            if (objA == nullptr) // if not, skip
            {
                continue;
            }

            for (size_t b = a + 1; b < mGameObjects.size(); ++b) {// checking objects after a to avoid checking same pair
                auto objB = std::dynamic_pointer_cast<CollisionObject>(mGameObjects[b]);
                if (objB == nullptr) {
                    continue;
                }

                // get each obj's collission rectangle bounds
                // using copies to not modify rectangle that will happen
                Rect boundsA = objA->GetBounds();
                Rect boundsB = objB->GetBounds();

                // replace boundsA with area where A and B overlap
                boundsA &= boundsB;

                if (boundsA.width > 0 && boundsA.height > 0) {// if 2 rectangles made positive overlapped area
                    // notifies both objects about collision
                    objA->CollisionEnter(objB);
                    objB->CollisionEnter(objA);
                }
            }
        }

        // 5. Late updates
        // LateUpdate runs after all regular updates and collision processing.
        // This allows objects to perform actions that depend on the results
        // of the current frame's updates and collisions.
        for (auto gameObject : mGameObjects) {
            gameObject->LateUpdate(&context); // calls each object's latest update
        }

        // Clear window
        mWindow->clear();

        // 6. Render background
        // Background graphics are drawn first so that foreground objects
        // appear on top of them.
        for (auto gameObject : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject); // checks if gameobj also graphicsobj

            if (graphicsObject != nullptr) { // if so, call render func
                graphicsObject->RenderBackground(&context);
            }
        }

        // 7. Render foreground
        // Foreground graphics are drawn after the background so game objects
        // such as the player, enemies, and bullets appear on top.
        for (auto gameObject : mGameObjects) {
            auto graphicsObject = std::dynamic_pointer_cast<GraphicsObject>(gameObject);

            if (graphicsObject != nullptr)
            {
                graphicsObject->RenderForeground(&context);
            }
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