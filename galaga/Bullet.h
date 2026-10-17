#ifndef BULLET_H
#define BULLET_H

#include <memory>

#include "CollisionObject.h"
#include "GameContext.h"
#include "GameObject.h"

class Bullet : public CMPUT350::CollisionObject
{
public:
    Bullet(CMPUT350::Point2D location, CMPUT350::Vector2D heading, bool player,
           std::weak_ptr<CMPUT350::GameObject> parent);
    bool IsPlayerBullet();

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override; 

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;

    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj, const CMPUT350::Point2D& collisionPoint) override;

    const std::vector<CMPUT350::Shape>& GetShapes() override;

private:
    CMPUT350::Point2D mLocation;  // Current position of the bullet, in screen pixels
    CMPUT350::Vector2D mHeading;   // Distance moved per frame; negative y moves up
    bool mPlayerBullet;           // True if fired by the player, false if by an enemy
    CMPUT350::Rect mBounds;       // Box covering the previous and current position, used for collisions
    bool mBulletAlive;            // False once Kill has been called; the engine then removes the bullet
    std::vector<CMPUT350::Shape> mShapes;
    CMPUT350::Point2D mHitPoint;  // Where a hit killed the bullet, in its local coordinates
    bool mHasHit = false;         // True once a hit killed the bullet; the circle is drawn then
    std::weak_ptr<CMPUT350::GameObject> mParent;  // Object that fired this bullet; the bullet dies with it

};
#endif // BULLET_H