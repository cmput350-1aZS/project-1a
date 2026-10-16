#ifndef GALAGA_H
#define GALAGA_H

#include <string>

#include "GameObject.h"

/**
 * @brief The states of the Galaga game manager
 *
 * Waiting: waits for coins (5) and a start key (1).
 * Startup: shows the level and the score while the player ship appears.
 * Waves: spawns 5 waves of 8 enemies, one wave per second.
 * GamePlay: runs until every enemy is dead, then goes back to Startup.
 * GameOver: shows GAME OVER for a few seconds, then goes back to Waiting.
 */
enum class State { Waiting, Startup, Waves, GamePlay, GameOver };

/**
 * @brief The game manager: a state machine that runs a whole game of Galaga
 *
 * The engine holds one Galaga object. It listens for key and enemy/player notifications, draws
 * the GUI (prompt, score, lives, messages), and creates and removes the player and the enemies.
 * Enemies and the player never talk to Galaga directly; they only announce when they die.
 */
class Galaga : public CMPUT350::GameObject {
public:
    void Initialize(CMPUT350::GameContext* context) override;
    void RenderUI(CMPUT350::GameContext* context) override;
    void ReceiveNotification(const std::string& key) override;
    void Update(CMPUT350::GameContext* context) override;

private:
    State mState = State::Waiting;  // Current state of the game
    int mCredits = 0;               // Coins inserted with the 5 key; a game needs at least one
    bool mPlayerSpawned = false;    // True while a player ship exists; reset when the ship dies
    int mScore = 0;                 // Points earned in the current game
    int mLives = 3;                 // Ships left; the game ends when this reaches 0
    int mGameOverFrames = 0;        // Frames spent on the GAME OVER screen
    int mStartupFrames = 0;         // Frames spent on the LEVEL screen
    int mWaveFrames = 0;            // Frames since the last wave was spawned
    int mWavesSpawned = 0;          // Waves spawned so far in this level (0 to 5)
    int mEnemiesAlive = 0;          // Enemies spawned and not yet destroyed
};

#endif
