#include "Player.h"

#include <cassert>

#include "Bullet.h"
#include "Enemy.h"
#include "NotificationManager.h"
/**
 * @brief Constructs a player at the given location.
 *
 * @param loc The initial position of the player.
 */
Player::Player(CMPUT350::Point2D loc) : mLoc(loc), mAlive(true) {
    mPlayerShapes.push_back(CMPUT350::Rect(-20, -20, 40, 40));
}

/**
 * @brief Initializes the player after it is added to the game engine.
 *
 * Registers the player for the "MoveLeft", "MoveRight" and "Fire" messages, and remembers a weak
 * pointer to itself so the bullets it fires can name it as their parent.
 *
 * @param context The game context containing access to the engine and screen.
 */
void Player::Initialize(CMPUT350::GameContext* context) {
    context->mNotificationManager->Register(context->CurrObject, "MoveLeft");
    context->mNotificationManager->Register(context->CurrObject, "MoveRight");
    context->mNotificationManager->Register(context->CurrObject, "Fire");
    mSelf = context->CurrObject;
}

/**
 * @brief Updates the player's state for the current frame.
 *
 * Removes weak pointers to bullets that no longer exist. This keeps the
 * player's bullet list limited to bullets that are still owned by the engine. If a "Fire" message
 * arrived since the last frame, a new bullet is made, but only while fewer than two player bullets
 * are alive.
 *
 * @param context The game context containing access to the engine and screen.
 */
void Player::Update(CMPUT350::GameContext* context) {
    // removes dead bullets

    auto it = mBullets.begin();  // iterator that points to first element in vector

    while (it != mBullets.end()) {
        if (it->expired())  // it->expired becomes true if bullet is destroyed
        {
            it = mBullets.erase(it);  // removes expired bullet from players vector
        } else {
            ++it;  // if not expired, moves to next bullet
        }
    }
    if (mFire) {
        mFire = false;
        if (mBullets.size() < 2) {
            auto bullet = std::make_shared<Bullet>(CMPUT350::Point2D(mLoc.x, mLoc.y - 20),
                                                   CMPUT350::Vector2D(0, -10), true, mSelf);
            context->mEngineView->AddGameObject(bullet);
            mBullets.push_back(bullet);
        }
    }
}

/**
 * @brief Performs updates after the main update and collision stages.
 *
 * Sends "PlayerDied" to Galaga once the player has been killed. The engine removes the dead
 * player at the start of the next frame, so the message is sent exactly once.
 *
 * @param context The game context containing access to the engine and screen.
 */
void Player::LateUpdate(CMPUT350::GameContext* context) {
    if (!mAlive) {
        context->mNotificationManager->Notify("PlayerDied");
    }
}

/**
 * @brief Renders the player's background graphics.
 *
 * @param context The game context containing access to the screen.
 */
void Player::RenderBackground(CMPUT350::GameContext* context) {}

/**
 * @brief Returns the player's screen position, which the engine uses as the ship's local origin
 */
CMPUT350::Point2D Player::GetLocation() const { return mLoc; }

/**
 * @brief Returns the ship's rotation in radians; the player ship never rotates
 */
float Player::GetRotation() const { return 0; }

/**
 * @brief Renders the player's ship in the foreground.
 *
 * The ship is drawn using four lines that form a triangular outline. The points are relative to
 * the ship's centre (0, 0); the engine's draw context shifts them to GetLocation().
 *
 * @param context The game context containing access to the screen.
 */
void Player::RenderForeground(CMPUT350::GameContext* context) {
    // triangle to form ship
    context->ScreenContext->DrawLine(CMPUT350::Point2D(0, -20), CMPUT350::Point2D(-20, 20), 4,
                                     CMPUT350::Colors::white);  // top pt
    context->ScreenContext->DrawLine(CMPUT350::Point2D(-20, 20), CMPUT350::Point2D(0, 10), 4,
                                     CMPUT350::Colors::white);  // bottom left
    context->ScreenContext->DrawLine(CMPUT350::Point2D(0, 10), CMPUT350::Point2D(20, 20), 4,
                                     CMPUT350::Colors::white);  // bottom centre
    context->ScreenContext->DrawLine(CMPUT350::Point2D(20, 20), CMPUT350::Point2D(0, -20), 4,
                                     CMPUT350::Colors::white);  // bottom right
}

/**
 * @brief Handles a collision with another collision object.
 *
 * The player is killed when it touches an Enemy or an enemy bullet. Its own bullets and
 * everything else are ignored.
 *
 * @param obj The collision object that collided with the player.
 */
void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) {
    std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(obj);
    if (enemy != nullptr) {
        Kill();
    }

    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr && !bullet->IsPlayerBullet()) {
        Kill();
    }
}

/**
 * @brief Marks the player as no longer alive.
 *
 * The game engine will remove the player when it is no longer alive.
 */
void Player::Kill() { mAlive = false; }

/**
 * @brief Checks whether the player is still alive.
 *
 * @return true if the player is alive, otherwise false.
 */
bool Player::IsAlive() const {
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
const CMPUT350::Rect& Player::GetBounds() {
    static CMPUT350::Rect sBounds(0, 0, 0, 0);

    // this is player's collision bounds
    sBounds = CMPUT350::Rect(mLoc.x - 20, mLoc.y - 20, 40, 40);

    return sBounds;
}

const std::vector<CMPUT350::Shape>& Player::GetShapes() { return mPlayerShapes; }

/**
 * @brief Reacts to the key messages "MoveLeft", "MoveRight" and "Fire"
 *
 * Each move message shifts the ship 10 pixels. The x position is then limited to 20..748, so the
 * 40 pixel wide ship stays inside the 768 pixel wide screen. "Fire" only sets mFire; Update makes
 * the bullet because it has the game context.
 *
 * @param key The message that was sent
 */
void Player::ReceiveNotification(const std::string& key) {
    if (key == "MoveLeft") {
        mLoc.x -= 10;
    } else if (key == "MoveRight") {
        mLoc.x += 10;
    } else if (key == "Fire") {
        mFire = true;
    }

    if (mLoc.x < 20) {
        mLoc.x = 20;
    } else if (mLoc.x > 748) {
        mLoc.x = 748;
    }
}