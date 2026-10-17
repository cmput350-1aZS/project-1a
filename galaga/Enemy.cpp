
#include "Enemy.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "Bezier.h"
#include "Bullet.h"
#include "NotificationManager.h"
#include "Player.h"

/**
 * @brief Constructs an enemy with a formation location and entry path.
 *
 * Enemies follow a Bezier path into formation, hover while waiting to attack,
 * and use additional Bezier paths to attack and return to formation.
 *
 * @param formationLocation The enemy's assigned formation location, in screen pixels
 * @param entryPath The Bezier control points defining the enemy's entry path
 * @param health The number of hits the enemy can take before being destroyed
 * @param score The score awarded when the enemy is destroyed
 * @param movementSpeed The approximate distance the enemy moves per frame
 * @param attackCooldown The number of frames between attacks
 * @param bulletSpeed The speed of bullets fired by this enemy
 * @param attackArcX The horizontal distance used to shape the attack path
 * @param attackArcY The vertical distance used to shape the attack path
 * @param hoverPhase The phase offset used for synchronized formation hovering
 * @param enemyType The enemy type used in destruction notifications
 */
Enemy::Enemy(
    CMPUT350::Point2D formationLocation,
    const std::vector<CMPUT350::Point2D>& entryPath,
    int health,
    int score,
    float movementSpeed,
    int attackCooldown,
    float bulletSpeed,
    float attackArcX,
    float attackArcY,
    float hoverPhase,
    const std::string& enemyType
)
    : mLocation(formationLocation),
      mFormationLocation(formationLocation),
      mEnemyBounds(formationLocation, 8.0f),
      mMovementSpeed(movementSpeed),
      mBulletSpeed(bulletSpeed),
      mAttackArcX(attackArcX),
      mAttackArcY(attackArcY),
      mHoverPhase(hoverPhase),
      mHealth(health),
      mScore(score),
      mAttackCooldown(attackCooldown),
      mEnemyType(enemyType)
{
    std::vector<CMPUT350::Point2D> path = entryPath;

    // use default cubic beziedr path if the supplied path is not valid
    if (path.size() < 4 || (path.size() - 1) % 3 != 0) {
        path = {formationLocation + CMPUT350::Point2D(-180.0f, -200.0f), formationLocation + CMPUT350::Point2D(-100.0f, -220.0f), formationLocation + CMPUT350::Point2D(100.0f, -150.0f), formationLocation};
    }

    // start at the first pt of the entry path
    mLocation = path.front();

    SetPath(path);
}

/**
 * @brief Initializes the enemy.
 *
 * Stores the notification manager used to report enemy destruction.
 *
 * @param context Provides the game context used by the enemy
 */
void Enemy::Initialize(CMPUT350::GameContext* context) {
    mNotificationManager = context->mNotificationManager;
}

/**
 * @brief Updates the enemy's movement and attack behavior.
 *
 * Enemies first follow their entry paths. Once they enter formation,
 * they hover using the shared frame counter and wait until all enemies
 * have entered formation before beginning attacks.
 *
 * @param context Provides the game context used by the enemy
 */
void Enemy::Update(CMPUT350::GameContext* context) {
    if (!mAlive) {
        return;
    }

    // follow the current entry/attack path
    if (mPathActive) {
        AdvancePath();
        return;
    }

    if (!mInFormation) {
        return;
    }

    // hover in formation using the shared frame counter so that enemy movement remains in sync w others
    const float frame = static_cast<float>(context->currentFrame);
    const float hover = std::sin(frame * 0.04f + mHoverPhase) * 8.0f;

    mLocation = CMPUT350::Point2D(mFormationLocation.x + hover, mFormationLocation.y);

    // wait till all active enemy in formation
    if (!AllEnemiesHaveEnteredFormation(context)) {
        return;
    }

    // find player so the enemy can aim its attack
    CMPUT350::Point2D playerLocation;
    bool playerFound = false;

    for (auto it = context->mEngineView->begin();
         it != context->mEngineView->end(); ++it) {std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(*it);

        if (player != nullptr && player->IsAlive()) {
            playerLocation = player->GetLocation();
            playerFound = true;
            break;
        }
    }

    // Attack when the cooldown has expired.
    if (playerFound && context->currentFrame >= mNextAttackFrame) {
        Attack(context, playerLocation);
    }
}

/**
 * @brief Performs updates after the main update and collision stages.
 *
 * @param context Provides the game context used by the enemy
 */
void Enemy::LateUpdate(CMPUT350::GameContext* context) {
    (void)context;
}

//bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key) { return false; }

/**
 * @brief Sets up the control points and distance samples for a Bezier path.
 *
 * Samples the path and stores cumulative distances so movement can advance
 * by approximately the same distance each frame instead of advancing the
 * Bezier parameter by a constant amount.
 *
 * @param controlPoints The control points defining the path
 */
void Enemy::SetPath(
    const std::vector<CMPUT350::Point2D>& controlPoints
) {
    mPathControlPoints = controlPoints;
    mPathSamples.clear();
    mPathDistances.clear();

    mPathDistance = 0.0f;
    mPathLength = 0.0f;

    if (mPathControlPoints.size() < 4 || (mPathControlPoints.size() - 1) % 3 != 0) {
        mPathActive = false;
        return;
    }

    const std::size_t segments = (mPathControlPoints.size() - 1) / 3;
    const std::size_t samplesPerSegment = 100;
    const std::size_t sampleCount = segments * samplesPerSegment;

    mPathSamples.reserve(sampleCount + 1);
    mPathDistances.reserve(sampleCount + 1);

    for (std::size_t i = 0; i <= sampleCount; ++i) {
        const float t = static_cast<float>(segments) * static_cast<float>(i) / static_cast<float>(sampleCount);

        const CMPUT350::Point2D point = CMPUT350::Bezier::GetPoint(mPathControlPoints, t);

        mPathSamples.push_back(point);

        if (i == 0) {
            mPathDistances.push_back(0.0f);
        } else {
            const CMPUT350::Point2D difference = point - mPathSamples[i - 1];

            const float distance = std::sqrt(difference.x * difference.x + difference.y * difference.y);

            mPathLength += distance;
            mPathDistances.push_back(mPathLength);
        }
    }

    mPathActive = mPathLength > 0.0f;
}

/**
 * @brief Advances the enemy along its current path.
 *
 * Uses the cumulative distance samples to approximate constant-speed
 * movement along the Bezier curve.
 */
void Enemy::AdvancePath() {
    if (!mPathActive || mPathSamples.empty()) {
        return;
    }

    mPathDistance += mMovementSpeed;

    // Finish the path and return the enemy to formation.
    if (mPathDistance >= mPathLength) {
        mLocation = mPathSamples.back();
        mPathActive = false;
        mHasEnteredFormation = true;
        mInFormation = true;
        return;
    }

    auto upper = std::lower_bound(
        mPathDistances.begin(),
        mPathDistances.end(),
        mPathDistance
    );

    if (upper == mPathDistances.begin()) {
        mLocation = mPathSamples.front();
        return;
    }

    const std::size_t upperIndex =
        static_cast<std::size_t>(
            std::distance(mPathDistances.begin(), upper)
        );

    const std::size_t lowerIndex = upperIndex - 1;

    const float lowerDistance = mPathDistances[lowerIndex];
    const float upperDistance = mPathDistances[upperIndex];
    const float interval = upperDistance - lowerDistance;

    if (interval <= 0.0f) {
        mLocation = mPathSamples[upperIndex];
        return;
    }

    const float fraction =
        (mPathDistance - lowerDistance) / interval;

    mLocation = mPathSamples[lowerIndex] + (mPathSamples[upperIndex] - mPathSamples[lowerIndex]) * fraction;
}

/**
 * @brief Checks whether all active enemies have entered formation.
 *
 * @param context Provides access to the active game objects
 * @return True if every active enemy has entered formation
 */
bool Enemy::AllEnemiesHaveEnteredFormation(
    CMPUT350::GameContext* context
) const {
    for (auto it = context->mEngineView->begin();
         it != context->mEngineView->end(); ++it) {std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(*it);

        if (enemy != nullptr && enemy->IsAlive() && !enemy->mHasEnteredFormation) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Starts an attack path that returns to formation.
 *
 * @param context Provides the current frame for attack cooldowns
 * @param target The player's current location
 * @return True if the attack path was started
 */
bool Enemy::BeginAttack(
    CMPUT350::GameContext* context,
    const CMPUT350::Point2D& target
) {
    (void)target;

    if (!mAlive || !mInFormation || mPathActive) {
        return false;
    }

    // path leaves formation then sweeps through an attack motion then returns to the assigned location again
    const std::vector<CMPUT350::Point2D> attackPath = {mLocation, mLocation + CMPUT350::Point2D(-mAttackArcX, mAttackArcY), mFormationLocation + CMPUT350::Point2D(mAttackArcX, mAttackArcY), mFormationLocation};

    SetPath(attackPath);

    if (!mPathActive) {
        return false;
    }

    mInFormation = false;
    mNextAttackFrame = context->currentFrame + mAttackCooldown;

    return true;
}

/**
 * @brief Fires a bullet toward the target.
 *
 * @param context Provides the game engine and current game object
 * @param target The target location
 */
void Enemy::FireAt(
    CMPUT350::GameContext* context,
    const CMPUT350::Point2D& target
) {
    const CMPUT350::Point2D difference = target - mLocation;

    const float length = std::sqrt(difference.x * difference.x + difference.y * difference.y);

    if (length <= 0.0f) {
        return;
    }

    const CMPUT350::Point2D heading(difference.x / length * mBulletSpeed, difference.y / length * mBulletSpeed);

    // bullet tracks its parent so it can be removed when the enemy that fired it is dead
    auto bullet = std::make_shared<Bullet>(mLocation, heading, false, context->CurrObject);

    context->mEngineView->AddGameObject(bullet);
}

/**
 * @brief Draws the enemy as a filled rectangle.
 *
 * The rectangle is centered on (0, 0); the engine's draw context
 * shifts it to GetLocation().
 *
 * @param context Provides the draw context used to draw the rectangle
 * @param halfWidth Half of the rectangle's width
 * @param halfHeight Half of the rectangle's height
 */
void Enemy::DrawEnemy(
    CMPUT350::GameContext* context,
    float halfWidth,
    float halfHeight
) {
    context->ScreenContext->DrawRect(CMPUT350::Rect(CMPUT350::Point2D(-halfWidth, -halfHeight), CMPUT350::Point2D(halfWidth, halfHeight)), CMPUT350::Colors::yellow);
}

/**
 * @brief Reports whether the enemy is currently in formation.
 */
bool Enemy::IsInFormation() const {
    return mInFormation;
}

/**
 * @brief Draws the enemy's background graphics.
 *
 * @param context Provides the draw context used to draw the enemy
 */
void Enemy::RenderBackground(CMPUT350::GameContext* context) {
    (void)context;
}

/**
 * @brief Draws the enemy as a filled yellow rectangle.
 *
 * The enemy is drawn in local coordinates, and the engine's draw context
 * shifts it to GetLocation().
 *
 * @param context Provides the draw context used to draw the rectangle
 */
void Enemy::RenderForeground(CMPUT350::GameContext* context) {
    DrawEnemy(context, mSize, mSize);
}

/**
 * @brief Returns the enemy's screen position, which the engine uses as its local origin
 */
CMPUT350::Point2D Enemy::GetLocation() const {
    return mLocation;
}

/**
 * @brief Returns the enemy's rotation in radians; enemies never rotate
 */
float Enemy::GetRotation() const {
    return 0.0f;
}

/**
 * @brief Called by the engine when this enemy's box overlaps another collision object.
 *
 * Enemies are destroyed when they collide with the player. Player bullets
 * reduce their health, while enemy bullets and other enemies are ignored.
 *
 * @param obj The object this enemy overlapped
 */
void Enemy::CollisionEnter(
    const std::shared_ptr<CMPUT350::CollisionObject>& obj
) {
    if (!mAlive || obj == nullptr) {
        return;
    }

    // enemies are dead if they collide with the player
    std::shared_ptr<Player> player = std::dynamic_pointer_cast<Player>(obj);

    if (player != nullptr) {
        Kill();
        return;
    }

    // dynamic_pointer_cast returns nullptr unless obj really is a Bullet.
    std::shared_ptr<Bullet> bullet =
        std::dynamic_pointer_cast<Bullet>(obj);

    // only player bullets can damage enemy
    if (bullet != nullptr && bullet->IsPlayerBullet()) {
        bullet->Kill();
        --mHealth;

        if (mHealth <= 0) {
            Kill();
        }
    }
}

/**
 * @brief Marks the enemy as dead so the engine removes it at the start of the next frame.
 *
 * Sends a destruction notification containing the enemy type and score.
 * Enemies destroyed outside formation also send a formation-bonus notification.
 */
void Enemy::Kill() {
    if (!mAlive) {
        return;
    }

    mAlive = false;

    if (mNotificationManager != nullptr) {
        mNotificationManager->Notify("EnemyDestroyed_" + mEnemyType + "_" + std::to_string(mScore));

        if (mHasEnteredFormation && !mInFormation) {
            mNotificationManager->Notify("EnemyDestroyed_FormationBonus_50");
        }
    }
}

/**
 * @brief Reports whether the enemy is still in the game.
 *
 * @return True while the enemy is alive, false once Kill has been called
 */
bool Enemy::IsAlive() const {
    return mAlive;
}

/**
 * @brief Gives the engine the box used for collision checks.
 *
 * The box is updated to follow the enemy's current location.
 *
 * @return The collision box centered on the enemy's location
 */
const CMPUT350::Rect& Enemy::GetBounds() {
    mEnemyBounds = CMPUT350::Rect(mLocation, mSize);
    return mEnemyBounds;
}


// -----------------------------------------------------------------------------
// ScoutEnemy
// Fast enemy with one health point and a relatively short attack cooldown.
// -----------------------------------------------------------------------------

/**
 * @brief Constructs a scout enemy.
 */
// OPENAI IMPLEMENTATION STARTS HERE
ScoutEnemy::ScoutEnemy(
    CMPUT350::Point2D formationLocation,
    const std::vector<CMPUT350::Point2D>& entryPath
)
    : Enemy(
        formationLocation,
        entryPath,
        1,
        100,
        4.0f,
        75,
        5.0f,
        90.0f,
        150.0f,
        0.0f,
        "Scout"
    )
{
    mSize = 7.0f;
}
// OPENAI IMPLEMENTATION ENDS HERE

/**
 * @brief Starts a scout attack and fires toward the target.
 */
bool ScoutEnemy::Attack(
    CMPUT350::GameContext* context,
    const CMPUT350::Point2D& target
) {
    if (!BeginAttack(context, target)) {
        return false;
    }

    FireAt(context, target);
    return true;
}

/**
 * @brief Draws the scout with a diagonal accent.
 */
void ScoutEnemy::RenderForeground(CMPUT350::GameContext* context) {
    DrawEnemy(context, 7.0f, 7.0f);
    context->ScreenContext->DrawLine(CMPUT350::Point2D(-5.0f, -4.0f), CMPUT350::Point2D(5.0f, 4.0f), 2.0f, CMPUT350::Colors::blue);
}


// -----------------------------------------------------------------------------
// FighterEnemy
// Medium-speed enemy with one health point and a wider attack arc.
// -----------------------------------------------------------------------------

/**
 * @brief Constructs a fighter enemy.
 */
FighterEnemy::FighterEnemy(
    CMPUT350::Point2D formationLocation,
    const std::vector<CMPUT350::Point2D>& entryPath
)
    : Enemy(
        formationLocation,
        entryPath,
        1,
        200,
        3.0f,
        105,
        4.5f,
        150.0f,
        220.0f,
        1.5f,
        "Fighter"
    )
{
    mSize = 9.0f;
}

/**
 * @brief Starts a fighter attack and fires toward the target.
 */
bool FighterEnemy::Attack(
    CMPUT350::GameContext* context,
    const CMPUT350::Point2D& target
) {
    if (!BeginAttack(context, target)) {
        return false;
    }

    FireAt(context, target);
    return true;
}

/**
 * @brief Draws the fighter with a horizontal accent.
 */
void FighterEnemy::RenderForeground(CMPUT350::GameContext* context) {
    DrawEnemy(context, 9.0f, 6.0f);

    context->ScreenContext->DrawLine(CMPUT350::Point2D(-7.0f, 0.0f), CMPUT350::Point2D(7.0f, 0.0f), 2.0f, CMPUT350::Colors::blue);
}


// -----------------------------------------------------------------------------
// TankEnemy
// Slow enemy with two health points and a higher score value.
// -----------------------------------------------------------------------------

/**
 * @brief Constructs a tank enemy.
 */
TankEnemy::TankEnemy(
    CMPUT350::Point2D formationLocation,
    const std::vector<CMPUT350::Point2D>& entryPath
)
    : Enemy(
        formationLocation,
        entryPath,
        2,
        300,
        2.0f,
        150,
        3.5f,
        200.0f,
        260.0f,
        3.0f,
        "Tank"
    )
{
    mSize = 12.0f;
}

/**
 * @brief Starts a tank attack and fires toward the target.
 */
bool TankEnemy::Attack(
    CMPUT350::GameContext* context,
    const CMPUT350::Point2D& target
) {
    if (!BeginAttack(context, target)) {
        return false;
    }

    FireAt(context, target);
    return true;
}

/**
 * @brief Draws the tank with two diagonal accents.
 */
void TankEnemy::RenderForeground(CMPUT350::GameContext* context) {
    DrawEnemy(context, 12.0f, 9.0f);

    context->ScreenContext->DrawLine(CMPUT350::Point2D(-8.0f, -4.0f), CMPUT350::Point2D(8.0f, 4.0f), 2.0f, CMPUT350::Colors::blue);

    context->ScreenContext->DrawLine(CMPUT350::Point2D(-8.0f, 4.0f), CMPUT350::Point2D(8.0f, -4.0f), 2.0f, CMPUT350::Colors::blue);
}