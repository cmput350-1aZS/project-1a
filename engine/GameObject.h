#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

namespace CMPUT350 {

class GameContext;

class GameObject {

private:

    // Tracks whether this object should remain active in the game engine.
    bool alive = true;

public:

    /**
     * @brief Destroys the game object.
     *
     * The destructor is virtual so derived game objects are destroyed
     * correctly when accessed through a GameObject pointer.
     */
    virtual ~GameObject() = default;

    /**
     * @brief Initializes the game object after it is added to the engine.
     *
     * @param context The game context containing access to the engine and screen.
     */
    virtual void Initialize(GameContext *context);

    /**
     * @brief Updates the game object's state for the current frame.
     *
     * @param context The game context containing access to the engine and screen.
     */
    virtual void Update(GameContext *context);

    /**
     * @brief Performs updates after the main update and collision stages.
     *
     * @param context The game context containing access to the engine and screen.
     */
    virtual void LateUpdate(GameContext *context);

    /**
     * @brief Renders user-interface elements for the game object.
     *
     * @param context The game context containing access to the engine and screen.
     */
    virtual void RenderUI(GameContext *context);

    /**
     * @brief Checks whether the game object is still active.
     *
     * @return true if the object is alive, otherwise false.
     */
    virtual bool IsAlive() const;

    /**
     * @brief Marks the game object as no longer active.
     *
     * The engine removes objects that are no longer alive at the beginning
     * of the next frame.
     */
    virtual void Kill();

    /**
 * @brief Handles a notification received from the notification manager.
 *
 * Derived classes can override this function to respond to specific
 * notifications, such as player movement or firing commands.
 *
 * @param key The notification message received.
 */
virtual void ReceiveNotification(const std::string& key);
};

}  // namespace CMPUT350

#endif  // GAMEOBJECT_H