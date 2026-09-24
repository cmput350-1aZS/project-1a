#include "Bullet.h"
#include "Enemy.h"

/**
 * @brief Constructs a bullet at a starting position that moves by a fixed step each frame
 *
 * @param location Starting position of the bullet, in screen pixels
 * @param heading Distance the bullet moves per frame (x and y); a negative y moves it up
 * @param player True if the player fired this bullet, false if an enemy did
 */
Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player)
{
    mLocation = location;
    mHeading = heading;
    mPlayerBullet = player;
    mBulletAlive = true;
    // The bullet has not moved yet, so its previous and current positions are the same
    // and the collision box starts out with zero size
    mBounds = CMPUT350::Rect(location, location);
}

/**
 * @brief Reports which side fired this bullet
 *
 * @return True if the player fired it, false if an enemy did
 */
bool Bullet::IsPlayerBullet()
{
    if (mPlayerBullet) {
        return true;
    }
    return false;
}


void Bullet::Initialize(CMPUT350::GameContext* context)
{
    // Nothing to do: the constructor already sets up everything a bullet needs
}

/**
 * @brief Moves the bullet one step along its heading and refreshes its collision box
 *
 * The bullet removes itself once it leaves the top of the screen so it does not
 * linger in the engine's object list forever.
 *
 * @param context Unused
 */
void Bullet::Update(CMPUT350::GameContext* context)
{
    // Remember where the bullet was before it moves
    CMPUT350::Point2D old = mLocation;
    mLocation += mHeading;
    // The box spans the path travelled this frame, so a fast bullet cannot skip over a thin target
    mBounds = CMPUT350::Rect(old, mLocation);

    mBounds.Inset(-2); // Negative inset grows the box by 2 pixels per side, so a straight-up bullet has some width to overlap with

    // y = 0 is the top of the screen, so y < 0 means the bullet has flown off it
    if (mLocation.y < 0) {
        Kill();
    }

}


void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
    // Nothing to do: all of a bullet's per-frame work happens in Update
}

/**
 * @brief Called for every key press; a bullet ignores keys
 *
 * @param context Unused
 * @param key Unused
 * @return Always false, meaning the bullet did not use the key
 */
bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

/**
 * @brief Draws the background layer; a bullet has no background graphics
 *
 * @param context Unused
 */
void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
    // Nothing to do: the bullet is drawn in the foreground pass only
}

/**
 * @brief Draws the bullet as a short line ending at its current position
 *
 * The other end of the line is one step behind, giving the bullet a small tail.
 *
 * @param context Provides the draw context used to draw the line
 */
void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    // AI-assisted: this DrawLine call was provided by Claude when asked how to draw the bullet as a line
    context->ScreenContext->DrawLine(mLocation, mLocation - mHeading, 2, CMPUT350::Colors::blue);
}

/**
 * @brief Called by the engine when this bullet's box overlaps another collision object
 *
 * The bullet is removed when it hits an Enemy and ignores everything else, such as
 * the player who fired it or other bullets.
 *
 * @param obj The object this bullet overlapped
 */
void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    // dynamic_pointer_cast returns nullptr unless obj really is an Enemy
    std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(obj);

    if (enemy != nullptr) {
        Kill();
    }

}

/**
 * @brief Marks the bullet as dead so the engine removes it at the start of the next frame
 */
void Bullet::Kill()
{
    mBulletAlive=false;
}

/**
 * @brief Reports whether the bullet is still active
 *
 * @return True while the bullet is alive, false once Kill has been called
 */
bool Bullet::IsAlive() const
{
    if (mBulletAlive) {
        return true;
    }
    return false;
}

/**
 * @brief Gives the engine the box used for collision checks
 *
 * @return The box covering the bullet's previous and current position
 */
const CMPUT350::Rect& Bullet::GetBounds()
{
    return mBounds;
}
