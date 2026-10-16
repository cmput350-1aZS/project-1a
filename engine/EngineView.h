
#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>
#include <vector>
#include <string>

namespace CMPUT350 {

class GameObject;

/**
 * @brief Provides an interface for interacting with the game engine.
 *
 * GameEngine inherits from this class so that game objects can request
 * that new objects be added and keyboard notifications be registered
 * without needing direct access to the GameEngine class.
 */
class EngineView {
public:
    using tGameObject = std::vector<std::shared_ptr<GameObject>>;
    using const_iterator = tGameObject::const_iterator;

    virtual ~EngineView() = default;

    /**
     * @brief Adds a game object to the game engine.
     *
     * @param gameObject The game object to add to the engine.
     *
     * The game object is added to the engine's waiting list and will be
     * activated during the next game-engine frame.
     */
    virtual void AddGameObject(
        std::shared_ptr<GameObject> gameObject) = 0;

    /**
     * @brief Registers a notification for a key press.
     *
     * @param key The keyboard scancode.
     * @param notification The notification message to send.
     */
    virtual void InstallKeyDownNotification(
        int key, const std::string& notification) = 0;

    /**
     * @brief Registers a notification for a key release.
     *
     * @param key The keyboard scancode.
     * @param notification The notification message to send.
     */
    virtual void InstallKeyUpNotification(
        int key, const std::string& notification) = 0;

    /**
     * @brief Returns an iterator to the first game object.
     */
    virtual const_iterator begin() const = 0;

    /**
     * @brief Returns an iterator past the last game object.
     */
    virtual const_iterator end() const = 0;

    /**
     * @brief Returns an iterator to the first game object.
     */
    virtual const_iterator cbegin() const = 0;

    /**
     * @brief Returns an iterator past the last game object.
     */
    virtual const_iterator cend() const = 0;
};

}  // namespace CMPUT350

#endif
