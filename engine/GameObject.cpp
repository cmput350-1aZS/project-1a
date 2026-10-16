#include "GameObject.h"
#include <string>

namespace CMPUT350 {

/**
 * @brief Initializes the game object.
 *
 * This default implementation does nothing. Derived game objects can override
 * this function when they need to perform initialization after being added to
 * the game engine.
 *
 * @param context The game context containing access to the engine and screen.
 *
 * This function does not return a value.
 */
void GameObject::Initialize(GameContext *context) {
    return;
}

/**
 * @brief Updates the game object's state for the current frame.
 *
 * This default implementation does nothing. Derived game objects can override
 * this function to perform their per-frame updates.
 *
 * @param context The game context containing access to the engine and screen.
 *
 * This function does not return a value.
 */
void GameObject::Update(GameContext *context) {
    return;
}

/**
 * @brief Performs updates after the main update and collision stages.
 *
 * This default implementation does nothing. Derived game objects can override
 * this function when they need to perform actions after the regular update.
 *
 * @param context The game context containing access to the engine and screen.
 *
 * This function does not return a value.
 */
void GameObject::LateUpdate(GameContext *context) {
    return;
}

/**
 * @brief Renders user-interface elements for the game object.
 *
 * This default implementation does nothing. Derived game objects can override
 * this function when they need to render UI elements.
 *
 * @param contextrender The game context containing access to the screen.
 *
 * This function does not return a value.
 */
void GameObject::RenderUI(GameContext *contextrender) {
    return;
}

/**
 * @brief Checks whether the game object is still active.
 *
 * @return true if the object is alive, or false if Kill() has been called.
 */
bool GameObject::IsAlive() const {
    return alive;
}

/**
 * @brief Marks the game object as no longer active.
 *
 * The game engine removes objects that report false from IsAlive() at the
 * beginning of the next frame.
 *
 * This function does not return a value.
 */
void GameObject::Kill() {
    alive = false;
}


/**
 * @brief Handles a notification received from the notification manager.
 *
 * This default implementation does nothing. Derived game objects can
 * override this function to respond to specific notifications.
 *
 * @param key The notification message received.
 *
 * This function does not return a value.
 */
void GameObject::ReceiveNotification(const std::string& key) {
    return;
}


}  // namespace CMPUT350