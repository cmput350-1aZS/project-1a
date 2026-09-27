#include "Enemy.h"

#include "Bullet.h"

/**
 * @brief Constructs a stationary enemy centered on a point
 *
 * Enemies never move in Part 1a, so the collision box is built once here and never changes
 *
 * @param loc Center of the enemy, in screen pixels
 */
Enemy::Enemy(CMPUT350::Point2D loc) {
    mEnemyLocation = loc;
    mEnemyAlive = true;
    mEnemyBounds = CMPUT350::Rect(loc, 8.0f);
}

void Enemy::Initialize(CMPUT350::GameContext* context) {}

void Enemy::Update(CMPUT350::GameContext* context) {}

void Enemy::LateUpdate(CMPUT350::GameContext* context) {}

bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key) { return false; }

void Enemy::RenderBackground(CMPUT350::GameContext* context) {}

/**
 * @brief Draws the enemy as a filled yellow rectangle
 *
 * The drawn rectangle is the same box used for collisions, so what the player sees is what can be
 * hit.
 *
 * @param context Provides the draw context used to draw the rectangle
 */
void Enemy::RenderForeground(CMPUT350::GameContext* context) {
    context->ScreenContext->DrawRect(mEnemyBounds, CMPUT350::Colors::yellow);
}

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
