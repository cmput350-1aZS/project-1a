#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>
#include <vector>

namespace CMPUT350 {

class GameObject;

/**
 * @brief Provides an interface for adding game objects to the game engine.
 *
 * GameEngine inherits from this class so that game objects can request that
 * new objects be added without needing direct access to the GameEngine class.
 */
class EngineView {
public:
    /**
     * @brief Adds a game object to the game engine.
     *
     * @param gameObject The game object to add to the engine.
     *
     * The game object is added to the engine's waiting list and will be
     * activated during the next game-engine frame.
     */
    virtual void AddGameObject(std::shared_ptr<GameObject> gameObject) = 0;
};

}  // namespace CMPUT350

#endif