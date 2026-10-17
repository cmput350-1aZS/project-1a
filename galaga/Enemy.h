#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"
#include "Bezier.h"

class Enemy : public CMPUT350::CollisionObject {
public:
    Enemy(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D>& path);

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

    const std::vector<CMPUT350::Shape>& GetShapes() override;

    /**
     * @brief Asks the enemy to start an attack on a target
     *
     * Pure virtual (= 0): Enemy has no attack of its own, so every concrete enemy type must supply
     * one, and Enemy itself can no longer be created directly.
     *
     * @param context Gives access to the engine, for example to add bullets
     * @param target The point the enemy should attack, usually the player's position
     * @return True if the enemy began an attack, false if it cannot attack right now
     */
    virtual bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) = 0;

    /**
     * @brief Reports whether the enemy has reached its formation spot
     *
     * @return True once the enemy has finished its entry path
     */
    bool InFormation() const;

private:
    CMPUT350::Point2D mEnemyLocation;  // Center of the enemy, in screen pixels
    bool mEnemyAlive;  // False once Kill has been called; the engine then removes the enemy
    CMPUT350::Rect
        mEnemyBounds;  // Fixed box centered on mEnemyLocation, used for drawing and collisions
    
    std::vector<CMPUT350::Shape> mEnemyShapes;

    std::vector<CMPUT350::Point2D> mPath;
    float mT;
    CMPUT350::Point2D mFormationLoc;
    bool mInFormation;
};

/**
 * @brief The first concrete enemy type
 *
 * It inherits all the movement, drawing and collision behavior of Enemy and supplies the Attack
 * function that Enemy leaves unfinished. Enemy2 and Enemy3 will follow the same pattern.
 */
class Enemy1 : public Enemy {
public:
    Enemy1(CMPUT350::Point2D loc, const std::vector<CMPUT350::Point2D> &path):Enemy(loc, path){}
    virtual bool Attack(CMPUT350::GameContext *context, const CMPUT350::Point2D &target) override;

};
#endif
