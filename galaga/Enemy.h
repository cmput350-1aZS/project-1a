#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"

class Enemy : public CMPUT350::CollisionObject {
public:
    Enemy(CMPUT350::Point2D loc);

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    bool HandleKeyEvent(CMPUT350::GameContext* context, char key) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;

private:
    CMPUT350::Point2D mEnemyLocation;  // Center of the enemy, in screen pixels
    bool mEnemyAlive;                  // False once Kill has been called; the engine then removes the enemy
    CMPUT350::Rect mEnemyBounds;       // Fixed box centered on mEnemyLocation, used for drawing and collisions
};

#endif
