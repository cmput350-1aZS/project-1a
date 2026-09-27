#ifndef PLAYER_H
#define PLAYER_H

#include "CollisionObject.h"
#include <memory>
#include <vector>

class Bullet;

/**
 * @brief Represents the player's ship in the game.
 *
 * The player can move, fire bullets, be rendered, and participate in
 * collision detection.
 */
class Player : public CMPUT350::CollisionObject
{
public:
    /**
     * @brief Constructs a player at the given location.
     *
     * @param loc The initial position of the player.
     */
    Player(CMPUT350::Point2D loc);

    // GameObject Functions

    /**
     * @brief Initializes the player.
     *
     * @param context The game context used by the player.
     */
    void Initialize(CMPUT350::GameContext* context) override;

    /**
     * @brief Updates the player's state for the current frame.
     *
     * @param context The game context used by the player.
     */
    void Update(CMPUT350::GameContext* context) override;

    /**
     * @brief Performs updates after the main update and collision stages.
     *
     * @param context The game context used by the player.
     */
    void LateUpdate(CMPUT350::GameContext* context) override;

    /**
     * @brief Handles keyboard input for the player.
     *
     * @param context The game context used by the player.
     * @param key The keyboard character received by the player.
     *
     * @return true if the player handled the key, otherwise false.
     */
    bool HandleKeyEvent(CMPUT350::GameContext* context, char key) override;

    /**
     * @brief Checks whether the player is still alive.
     *
     * @return true if the player is alive, otherwise false.
     */
    bool IsAlive() const override;

    /**
     * @brief Marks the player as no longer alive.
     */
    void Kill() override;

    // Graphics Object Functions

    /**
     * @brief Renders the player's background graphics.
     *
     * @param context The game context used for rendering.
     */
    void RenderBackground(CMPUT350::GameContext* context) override;

    /**
     * @brief Renders the player's foreground graphics.
     *
     * @param context The game context used for rendering.
     */
    void RenderForeground(CMPUT350::GameContext* context) override;


    // Collision Object Functions

    /**
     * @brief Handles a collision with another collision object.
     *
     * @param obj The collision object that collided with the player.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;

    /**
     * @brief Gets the player's collision bounds.
     *
     * @return A reference to the rectangle representing the player's collision bounds.
     */
    const CMPUT350::Rect& GetBounds() override;

private:
    // The player's current position.
    CMPUT350::Point2D mLoc;

    // Tracks whether the player is still alive.
    bool mAlive; // mAlive because it is overridden above

    // Stores weak references to the player's bullets without owning them.
    std::vector<std::weak_ptr<Bullet>> mBullets; // player's bullet vector to observe but not own bullets

};

#endif