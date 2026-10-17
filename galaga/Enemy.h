#ifndef ENEMY_H
#define ENEMY_H

#include <memory>
#include <string>
#include <vector>

#include "CollisionObject.h"
#include "GameContext.h"
#include "Bezier.h"
#include "CollisionObject.h"
#include "GameContext.h"

class Enemy : public CMPUT350::CollisionObject {
public:
    Enemy(
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
    );

    ~Enemy() override = default;

    // GameObject Functions
    void Initialize(CMPUT350::GameContext* context) override;
    void Update(CMPUT350::GameContext* context) override;
    void LateUpdate(CMPUT350::GameContext* context) override;
    //bool HandleKeyEvent(CMPUT350::GameContext* context, char key) override;
    bool IsAlive() const override;
    void Kill() override;

    // Graphics Object Functions
    void RenderBackground(CMPUT350::GameContext* context) override;
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    const CMPUT350::Rect& GetBounds() override;

    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;

    // Merge fix: the collision API from the 1b engine requires every collision object to give shapes
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    // Public (was protected) so Galaga can check that every enemy is home before respawning the player
    bool IsInFormation() const;

    // returns true when this enemy starts attack
    virtual bool Attack(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    ) = 0;

private:
    void SetPath(const std::vector<CMPUT350::Point2D>& controlPoints);
    void AdvancePath();
    bool AllEnemiesHaveEnteredFormation(
        CMPUT350::GameContext* context
    ) const;

    CMPUT350::Point2D mLocation;
    CMPUT350::Point2D mFormationLocation;
    CMPUT350::Rect mEnemyBounds;
    std::vector<CMPUT350::Shape> mEnemyShapes;  // The one rectangle returned by GetShapes

    std::vector<CMPUT350::Point2D> mPathControlPoints;
    std::vector<CMPUT350::Point2D> mPathSamples;
    std::vector<float> mPathDistances;

    float mPathDistance = 0.0f;
    float mPathLength = 0.0f;

    float mMovementSpeed;
    float mBulletSpeed;
    float mAttackArcX;
    float mAttackArcY;
    float mHoverPhase;

    int mHealth;
    int mScore;
    int mAttackCooldown;
    int mNextAttackFrame = 0;

    bool mAlive = true;
    bool mPathActive = false;
    bool mHasEnteredFormation = false;
    bool mInFormation = false;

    std::string mEnemyType;

    CMPUT350::NotificationManager* mNotificationManager = nullptr;

protected:
    bool BeginAttack(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    );

    void FireAt(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    );

    void DrawEnemy(
        CMPUT350::GameContext* context,
        float halfWidth,
        float halfHeight
    );

    float mSize = 8.0f;
};

// type 1: fast, one health, relatively frequent attacks
class ScoutEnemy : public Enemy {
public:
    ScoutEnemy(
        CMPUT350::Point2D formationLocation,
        const std::vector<CMPUT350::Point2D>& entryPath
    );

    bool Attack(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    ) override;

    void RenderForeground(CMPUT350::GameContext* context) override;
};

// type 2: med speed, one health, different attack
class FighterEnemy : public Enemy {
public:
    FighterEnemy(
        CMPUT350::Point2D formationLocation,
        const std::vector<CMPUT350::Point2D>& entryPath
    );

    bool Attack(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    ) override;

    void RenderForeground(CMPUT350::GameContext* context) override;
};

// type 3: slow, two health
class TankEnemy : public Enemy {
public:
    TankEnemy(
        CMPUT350::Point2D formationLocation,
        const std::vector<CMPUT350::Point2D>& entryPath
    );

    bool Attack(
        CMPUT350::GameContext* context,
        const CMPUT350::Point2D& target
    ) override;

    void RenderForeground(CMPUT350::GameContext* context) override;
};

#endif
