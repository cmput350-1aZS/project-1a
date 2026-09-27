#include <cassert>
#include "Player.h"
#include "Bullet.h"

/**
 * @brief Constructs a player at the given location.
 *
 * @param loc The initial position of the player.
 */
Player::Player(CMPUT350::Point2D loc): mLoc(loc), mAlive(true)
{
    // TODO: Update code
}

/**
 * @brief Initializes the player after it is added to the game engine.
 *
 * @param context The game context containing access to the engine and screen.
 */
void Player::Initialize(CMPUT350::GameContext* context)
{
}

/**
 * @brief Updates the player's state for the current frame.
 *
 * Removes weak pointers to bullets that no longer exist. This keeps the
 * player's bullet list limited to bullets that are still owned by the engine.
 *
 * @param context The game context containing access to the engine and screen.
 */
void Player::Update(CMPUT350::GameContext* context)
{
    // removes dead bullets 

    auto it = mBullets.begin(); // iterator that points to first element in vector

    while (it != mBullets.end())
    {
        if (it->expired()) // it->expired becomes true if bullet is destroyed
        {
            it = mBullets.erase(it); // removes expired bullet from players vector
        }
        else
        {
            ++it; // if not expired, moves to next bullet
        }
    }
}

/**
 * @brief Performs updates after the main update and collision stages.
 *
 * @param context The game context containing access to the engine and screen.
 */
void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

/**
 * @brief Handles keyboard input for the player.
 *
 * The space key creates a player bullet when fewer than two bullets are
 * currently being tracked. The A and D keys move the player horizontally.
 *
 * @param context The game context containing access to the engine.
 * @param key The keyboard character received from the event.
 *
 * @return true if the player handled the key, otherwise false.
 */
bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    if (key == ' ') {
        if (mBullets.size() < 2){ // only if there are less than 2 bullets on screen
            // Create a bullet and give ownership to the engine with
            // Bullet(loc, movement, player bullet true (not enemy bullet)).
            auto bullet = std::make_shared<Bullet>(CMPUT350::Point2D(mLoc.x, mLoc.y -20), CMPUT350::Point2D(0,-10), true);
            context->mEngineView->AddGameObject(bullet); // allows game engine to handle bullet
            mBullets.push_back(bullet); // creates weak ptr that observes this bulletby player's mBullet vector (made of weak ptrs)
            return true;
        }
    }

    if (key == 'a' || key == 'A' ){ // left
        mLoc.x -=10;
        return true;
    }

    if (key == 'd' || key == 'D' ) { // right
        mLoc.x +=10;
        return true;
    }
    
    return false;
}

/**
 * @brief Renders the player's background graphics.
 *
 * @param context The game context containing access to the screen.
 */
void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

/**
 * @brief Renders the player's ship in the foreground.
 *
 * The ship is drawn using four lines that form a triangular outline.
 *
 * @param context The game context containing access to the screen.
 */
void Player::RenderForeground(CMPUT350::GameContext* context)
{
    // triangle to form ship
    context->ScreenContext->DrawLine(CMPUT350::Point2D(mLoc.x, mLoc.y-20), CMPUT350::Point2D(mLoc.x-20, mLoc.y+20), 4, CMPUT350::Colors::white); // top pt
    context->ScreenContext->DrawLine(CMPUT350::Point2D(mLoc.x-20, mLoc.y+20), CMPUT350::Point2D(mLoc.x, mLoc.y+10), 4, CMPUT350::Colors::white); // bottom left
    context->ScreenContext->DrawLine(CMPUT350::Point2D(mLoc.x, mLoc.y+10), CMPUT350::Point2D(mLoc.x+20, mLoc.y+20), 4, CMPUT350::Colors::white); // bottom centre
    context->ScreenContext->DrawLine(CMPUT350::Point2D(mLoc.x+20, mLoc.y+20), CMPUT350::Point2D(mLoc.x, mLoc.y-20), 4, CMPUT350::Colors::white); // bottom right
}

/**
 * @brief Handles a collision with another collision object.
 *
 * The player currently does not perform any action when a collision occurs.
 *
 * @param obj The collision object that collided with the player.
 */
void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
}

/**
 * @brief Marks the player as no longer alive.
 *
 * The game engine will remove the player when it is no longer alive.
 */
void Player::Kill()
{
    mAlive = false;
}

/**
 * @brief Checks whether the player is still alive.
 *
 * @return true if the player is alive, otherwise false.
 */
bool Player::IsAlive() const
{
    // TODO: Update code
    return mAlive;
}

/**
 * @brief Gets the player's collision bounds.
 *
 * The returned rectangle is centered on the player's current position and is
 * used by the engine for axis-aligned bounding-box collision detection.
 *
 * @return A reference to the player's collision bounding rectangle.
 */
const CMPUT350::Rect& Player::GetBounds()
{
    // TODO: Update code
    static CMPUT350::Rect sBounds(0, 0, 0, 0);

    // this is player's collision bounds
    sBounds = CMPUT350::Rect(mLoc.x-20, mLoc.y-20, 40, 40);

    return sBounds;
}