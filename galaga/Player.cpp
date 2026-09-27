#include <cassert>
#include "Player.h"
#include "Bullet.h"

Player::Player(CMPUT350::Point2D loc): mLoc(loc), mAlive(true)
{
    // TODO: Update code
}

void Player::Initialize(CMPUT350::GameContext* context)
{
}

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

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    if (key == ' ') {
        if (mBullets.size() < 2){ // only if there are less than 2 bullets on screen
            auto bullet = std::make_shared<Bullet>(CMPUT350::Point2D(mLoc.x, mLoc.y -20), CMPUT350::Point2D(0,-10), true); // created bullet and gives ownership to engine with Bullet(loc, movement, player bullet true (not enemy bullet))
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

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    // triangle to form ship
    context->ScreenContext->DrawLine(CMPUT350::Point2D(mLoc.x, mLoc.y-20), CMPUT350::Point2D(mLoc.x-20, mLoc.y+20), 4, CMPUT350::Colors::white); // top pt
    context->ScreenContext->DrawLine(CMPUT350::Point2D(mLoc.x-20, mLoc.y+20), CMPUT350::Point2D(mLoc.x, mLoc.y+10), 4, CMPUT350::Colors::white); // bottom left
    context->ScreenContext->DrawLine(CMPUT350::Point2D(mLoc.x, mLoc.y+10), CMPUT350::Point2D(mLoc.x+20, mLoc.y+20), 4, CMPUT350::Colors::white); // bottom centre
    context->ScreenContext->DrawLine(CMPUT350::Point2D(mLoc.x+20, mLoc.y+20), CMPUT350::Point2D(mLoc.x, mLoc.y-20), 4, CMPUT350::Colors::white); // bottom right

}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
}

void Player::Kill()
{
    mAlive = false;
}

bool Player::IsAlive() const
{
    // TODO: Update code
    return mAlive;
}

const CMPUT350::Rect& Player::GetBounds()
{
    // TODO: Update code
    static CMPUT350::Rect sBounds(0, 0, 0, 0);
    // this is player's collision bounds
    sBounds = CMPUT350::Rect(mLoc.x-20, mLoc.y-20, 40, 40);
    return sBounds;
}
