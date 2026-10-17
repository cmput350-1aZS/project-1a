#include "Galaga.h"

#include "Enemy.h"
#include "GameContext.h"
#include "NotificationManager.h"
#include "Player.h"
#include <SFML/Window/Keyboard.hpp>

/**
 * @brief Sets up the keys and notifications the manager reacts to
 *
 * Key 5 sends "InsertCoin" and key 1 sends "StartGame" (the MAME convention). Galaga also
 * registers for "EnemyDied" and "PlayerDied", which the enemies and the player send.
 *
 * The player's keys are also installed here: A sends "MoveLeft", D sends "MoveRight" and Space
 * sends "Fire". They are installed once by Galaga, not by each Player, because a new Player is
 * made after every death and the engine would otherwise hold the same key several times.
 *
 * @param context Gives access to the engine view and the notification manager
 */
void Galaga::Initialize(CMPUT350::GameContext* context) {
    context->mEngineView->InstallKeyDownNotification(static_cast<int>(sf::Keyboard::Scancode::Num5),
                                                     "InsertCoin");
    context->mEngineView->InstallKeyDownNotification(static_cast<int>(sf::Keyboard::Scancode::Num1),
                                                     "StartGame");
    context->mNotificationManager->Register(context->CurrObject, "InsertCoin");
    context->mNotificationManager->Register(context->CurrObject, "StartGame");
    context->mNotificationManager->Register(context->CurrObject, "EnemyDestroyed_Scout_100");
    context->mNotificationManager->Register(context->CurrObject, "EnemyDestroyed_Fighter_200");
    context->mNotificationManager->Register(context->CurrObject, "EnemyDestroyed_Tank_300");
    context->mNotificationManager->Register(context->CurrObject,
                                            "EnemyDestroyed_FormationBonus_50");
    context->mNotificationManager->Register(context->CurrObject, "PlayerDied");

    context->mEngineView->InstallKeyDownNotification(static_cast<int>(sf::Keyboard::Scancode::A),
                                                     "MoveLeft");
    context->mEngineView->InstallKeyDownNotification(static_cast<int>(sf::Keyboard::Scancode::D),
                                                     "MoveRight");
    context->mEngineView->InstallKeyDownNotification(
        static_cast<int>(sf::Keyboard::Scancode::Space), "Fire");
}

/**
 * @brief Draws the text for the current state
 *
 * Waiting shows the coin prompt, Startup shows the current level number, GameOver shows GAME OVER,
 * and every state except Waiting shows the score and the lives remaining. For about 3 seconds
 * after the player respawns, "LIVES LEFT n" is also shown (not during GameOver).
 *
 * @param context Provides the GUI draw context
 */
void Galaga::RenderUI(CMPUT350::GameContext* context) {
    if (mState == State::Waiting) {
        context->GUIContext->DrawCenteredText("PRESS 5 TO INSERT COIN", 32,
                                              CMPUT350::Point2D(384, 512), CMPUT350::Colors::white);
    }

    if (mState == State::Startup) {
        context->GUIContext->DrawCenteredText("LEVEL " + std::to_string(mLevel), 32,
                                              CMPUT350::Point2D(384, 512), CMPUT350::Colors::white);
    }

    if (mState != State::Waiting) {
        context->GUIContext->DrawText("SCORE " + std::to_string(mScore), 24,
                                      CMPUT350::Point2D(20, 20), CMPUT350::Colors::white);
        context->GUIContext->DrawText("LIVES " + std::to_string(mLives), 24,
                                      CMPUT350::Point2D(600, 20), CMPUT350::Colors::white);
    }

    if (mState == State::GameOver) {
        context->GUIContext->DrawCenteredText("GAME OVER", 32, CMPUT350::Point2D(384, 512),
                                              CMPUT350::Colors::white);
    }

    if (mRespawnFrames > 0 && mState != State::GameOver) {
        context->GUIContext->DrawCenteredText("LIVES LEFT " + std::to_string(mLives), 32,
                                              CMPUT350::Point2D(384, 512), CMPUT350::Colors::white);
    }
}

/**
 * @brief Reacts to a message sent through the notification manager
 *
 * "InsertCoin" adds a credit. "StartGame" starts a game if a coin was inserted. Enemies send
 * "EnemyDestroyed_<type>_<score>" when they die: the score is the number after the last
 * underscore, and the enemy count goes down by one. "EnemyDestroyed_FormationBonus_50" adds 50
 * more for an enemy killed out of formation and does not change the count. Both count only
 * while enemies are in play, so the clean-up at game over does not score. "PlayerDied" costs a
 * life and asks for a new ship.
 * The game is over once no lives are left.
 *
 * @param key The message that was sent
 */
void Galaga::ReceiveNotification(const std::string& key) {
    if (key == "InsertCoin") {
        mCredits += 1;
    } else if (key == "StartGame" && mState == State::Waiting && mCredits > 0) {
        mState = State::Startup;
        mCredits -= 1;
    } else if (key == "EnemyDestroyed_FormationBonus_50" &&
               (mState == State::Waves || mState == State::GamePlay)) {
        mScore += 50;
    } else if (key.starts_with("EnemyDestroyed_") &&
               (mState == State::Waves || mState == State::GamePlay)) {
        mScore += std::stoi(key.substr(key.rfind('_') + 1));
        mEnemiesAlive -= 1;
    } else if (key == "PlayerDied") {
        mLives -= 1;
        mPlayerSpawned = false;
    }

    if (mLives == 0) {
        mState = State::GameOver;
    }
}

/**
 * @brief Runs the state machine for one frame
 *
 * Spawns the player ship when one is needed and every enemy is in formation (so after a death
 * the new ship waits for the enemies to arrive), counts down the "LIVES LEFT" message timer,
 * then handles the timers and the exit condition of
 * the current state. Each state resets the counters it used when it ends, so the next pass
 * starts from zero.
 *
 * @param context Gives access to the engine view for adding and looping over game objects
 */
void Galaga::Update(CMPUT350::GameContext* context) {
    // allHome stays true only if no enemy is still flying in; with no enemies it is true
    bool allHome = true;
    for (auto object : *context->mEngineView) {
        std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(object);
        if (enemy != nullptr && !enemy->IsInFormation()) {
            allHome = false;
        }
    }
    // Make a new player ship whenever the game is running, there is none, and every enemy is home
    if (mState != State::Waiting && mState != State::GameOver && !mPlayerSpawned && allHome) {
        context->mEngineView->AddGameObject(
            std::make_shared<Player>(CMPUT350::Point2D(768 / 2, 900)));
        mPlayerSpawned = true;

        if (mLives < 3) {
            mRespawnFrames = 90;
        }
    }

    if (mRespawnFrames > 0) {
        mRespawnFrames -= 1;
    }

    // GameOver: show the message for 180 frames (about 3 seconds), then reset the whole game
    if (mState == State::GameOver) {
        mGameOverFrames += 1;
        if (mGameOverFrames >= 180) {
            mState = State::Waiting;
            mScore = 0;
            mLives = 3;
            mGameOverFrames = 0;
            mPlayerSpawned = false;
            mEnemiesAlive = 0;
            mWaveFrames = 0;
            mWavesSpawned = 0;
            mLevel = 1;
            mRespawnFrames = 0;

            // Remove the enemies that are still alive from the finished game
            for (auto object : *context->mEngineView) {
                std::shared_ptr<Enemy> enemy = std::dynamic_pointer_cast<Enemy>(object);
                if (enemy != nullptr) {
                    enemy->Kill();
                }
            }
        }
    }

    // Startup: show the level for 120 frames (about 2 seconds), then start the waves
    if (mState == State::Startup) {
        mStartupFrames += 1;
        if (mStartupFrames >= 120) {
            mState = State::Waves;
            mStartupFrames = 0;
        }
    }

    // Waves: every 60 frames add a row of 8 enemies; after 5 rows, switch to GamePlay
    if (mState == State::Waves) {
        mWaveFrames += 1;
        if (mWaveFrames >= 60) {
            mWaveFrames = 0;
            for (int i = 0; i < 8; i++) {
                CMPUT350::Point2D spot(10 + i * 50, 30 + mWavesSpawned * 40);
                std::vector<CMPUT350::Point2D> route = {CMPUT350::Point2D(spot.x, -50),
                                                        CMPUT350::Point2D(spot.x - 150, 400),
                                                        CMPUT350::Point2D(spot.x + 150, 400), spot};

                if (i % 3 == 0) {
                    context->mEngineView->AddGameObject(std::make_shared<ScoutEnemy>(spot, route));
                } else if (i % 3 == 1) {
                    context->mEngineView->AddGameObject(
                        std::make_shared<FighterEnemy>(spot, route));
                } else {
                    context->mEngineView->AddGameObject(std::make_shared<TankEnemy>(spot, route));
                }

                mEnemiesAlive += 1;
            }
            mWavesSpawned += 1;
            if (mWavesSpawned >= 5) {
                mState = State::GamePlay;
                mWavesSpawned = 0;
            }
        }
    }

    // GamePlay: when every enemy is destroyed, go back to Startup for the next level
    if (mState == State::GamePlay && mEnemiesAlive == 0) {
        mState = State::Startup;
        mLevel += 1;
    }
}