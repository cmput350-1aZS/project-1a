#ifndef GAMEENGINE_H
#define GAMEENGINE_H

namespace CMPUT350 {
class GameEngine;
}

#include "EngineView.h"
#include "GameObject.h"
#include "MathUtil.h"
#include <SFML/Graphics.hpp>

namespace CMPUT350 {

class DrawContext;

class GameEngine : public EngineView {
public:
    /**
     * @brief Constructs the game engine and creates its game window.
     *
     * @param width Width of the game window in pixels.
     * @param height Height of the game window in pixels.
     * @param name Title of the game window.
     */
    GameEngine(unsigned int width, unsigned int height, const std::string& name);

    /**
     * @brief Destroys the game engine and closes its window.
     */
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;             // Prevent copy-construction
    GameEngine(GameEngine&&) = delete;                  // Prevent move-construction
    GameEngine& operator=(const GameEngine&) = delete;  // Prevent assignment
    GameEngine& operator=(GameEngine&&) = delete;       // Prevent move-assignment

    /**
     * @brief Adds a game object to the list of objects waiting to be activated.
     *
     * The object is initialized and moved into the active object list at the
     * beginning of the next game-engine frame.
     *
     * @param gameObject The game object to add to the engine.
     */
    void AddGameObject(std::shared_ptr<GameObject> gameObject) override;

    /**
     * @brief Runs the main game loop.
     *
     * Each frame removes dead objects, activates newly added objects,
     * processes events, updates objects, checks collisions, performs late
     * updates, and renders the game.
     *
     * The function continues running until the game window is closed.
     */
    void Run();

private:
    // The window where the game is rendered and receives events.
    std::shared_ptr<sf::RenderWindow> mWindow;

    // Font used by the drawing system for rendering text.
    std::shared_ptr<sf::Font> mFont;

    // Game objects that are currently active in the engine.
    std::vector<std::shared_ptr<GameObject>> mGameObjects;

    // Game objects waiting to be activated at the beginning of the next frame.
    std::vector<std::shared_ptr<GameObject>> mAddedGameObjects;
};

}  // namespace CMPUT350

#endif  // GAMEENGINE_H