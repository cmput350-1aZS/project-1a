#include "Enemy.h"

#include <cmath>

#include "Bullet.h"
#include "NotificationManager.h"
/**
 * @brief Constructs an enemy that will fly along a Bezier path to its formation spot
 *
 * The enemy starts at the first control point of the path. Update moves it along the path and
 * snaps it to the formation spot at the end, after which it counts as being in formation.
 *
 * @param loc The enemy's spot in the formation, in screen pixels
 * @param path Control points of the Bezier curve the enemy follows to reach that spot
 */
Enemy::Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path) {
    mEnemyLocation = loc;
    mEnemyAlive = true;
    mEnemyBounds = CMPUT350::Rect(loc, 8.0f);
    mEnemyShapes.push_back(CMPUT350::Rect(CMPUT350::Point2D(0, 0), 8.0f));
    mPath = path;
    mT = 0;
    mFormationLoc = loc;
    mInFormation = false;
    mEnemyLocation = path[0];  // (the enemy starts at the first control point
}

void Enemy::Initialize(CMPUT350::GameContext* context) {}

/**
 * @brief Moves the enemy one step along its entry path
 *
 * While the enemy is not in formation, mT advances by 0.01 per frame and the enemy is placed at
 * that point of the Bezier curve; its collision box moves with it. When mT reaches 1 the enemy is
 * snapped to its formation spot and counts as in formation. Once in formation, the enemy hovers:
 * it swings up to 10 pixels left and right of its spot, following a sine of the engine's frame
 * number, so every enemy swings together.
 *
 * @param context Provides the current frame number for the hover
 */
void Enemy::Update(CMPUT350::GameContext* context) {
    if (!mInFormation) {
        mT += 0.01f;
        mEnemyLocation = CMPUT350::Bezier::GetPoint(mPath, mT);
        mEnemyBounds = CMPUT350::Rect(mEnemyLocation, 8.0f);

        if (mT >= 1.0f) {
            mInFormation = true;
            mEnemyLocation = mFormationLoc;
            mEnemyBounds = CMPUT350::Rect(mEnemyLocation, 8.0f);
        }
    } else {
        mEnemyLocation = CMPUT350::Point2D(
            mFormationLoc.x + 10 * std::sin(context->currentFrame * 0.1f), mFormationLoc.y);
        mEnemyBounds = CMPUT350::Rect(mEnemyLocation, 8.0f);
    }
}

/**
 * @brief Tells Galaga when this enemy has died
 *
 * LateUpdate runs after collisions, and the engine removes dead objects at the start of the
 * next frame, so the "EnemyDied" message is sent exactly once per enemy.
 *
 * @param context Provides the notification manager
 */
void Enemy::LateUpdate(CMPUT350::GameContext* context) {
    if (!mEnemyAlive) {
        context->mNotificationManager->Notify("EnemyDied");
    }
}

// bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key) { return false; }

void Enemy::RenderBackground(CMPUT350::GameContext* context) {}

/**
 * @brief Draws the enemy as a filled yellow rectangle
 *
 * The drawn rectangle is the same box used for collisions, so what the player sees is what can be
 * hit. It is centered on (0, 0); the engine's draw context shifts it to GetLocation().
 *
 * @param context Provides the draw context used to draw the rectangle
 */
void Enemy::RenderForeground(CMPUT350::GameContext* context) {
    context->ScreenContext->DrawRect(CMPUT350::Rect(CMPUT350::Point2D(0, 0), 8.0f),
                                     CMPUT350::Colors::yellow);
}

/**
 * @brief Returns the enemy's screen position, which the engine uses as its local origin
 */
CMPUT350::Point2D Enemy::GetLocation() const { return mEnemyLocation; }

/**
 * @brief Returns the enemy's rotation in radians; enemies never rotate
 */
float Enemy::GetRotation() const { return 0; }

/**
 * @brief Called by the engine when this enemy's box overlaps another collision object
 *
 * The enemy dies when a player Bullet hits it and ignores everything else, such as the player
 * or other enemies.
 *
 * @param obj The object this enemy overlapped
 */
void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) {
    // dynamic_pointer_cast returns nullptr unless obj really is a Bullet
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);

    if (bullet != nullptr && bullet->IsPlayerBullet()) {
        Kill();
    }
}

/**
 * @brief Marks the enemy as dead so the engine removes it at the start of the next frame
 */
void Enemy::Kill() { mEnemyAlive = false; }

/**
 * @brief Reports whether the enemy is still in the game
 *
 * @return True while the enemy is alive, false once Kill has been called
 */
bool Enemy::IsAlive() const {
    if (mEnemyAlive) {
        return true;
    }

    return false;
}

/**
 * @brief Gives the engine the box used for collision checks
 *
 * @return The 30x30 box centered on the enemy's location
 */
const CMPUT350::Rect& Enemy::GetBounds() { return mEnemyBounds; }

const std::vector<CMPUT350::Shape>& Enemy::GetShapes() { return mEnemyShapes; }

/**
 * @brief Reports whether the enemy has finished its entry path and reached its formation spot
 *
 * Galaga uses this to wait until every enemy is home before it respawns the player.
 *
 * @return True once the enemy is in formation, false while it is still flying in
 */
bool Enemy::InFormation() const { return mInFormation; }

/**
 * @brief Attack for the first enemy type
 *
 * Placeholder: this enemy cannot attack yet, so it always answers false.
 *
 * @param context Unused for now
 * @param target Unused for now
 * @return Always false
 */
bool Enemy1::Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) {
    return false;
}
