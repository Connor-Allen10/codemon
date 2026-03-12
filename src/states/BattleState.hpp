/**
 * @file BattleState.hpp
 * @brief Battle encounter state with simple placeholder UI.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "../game/State.hpp"
#include "../debug/DebugChallenge.hpp"
#include "../debug/ChallengeLoader.hpp"

class BattleState : public State {
public:
    explicit BattleState(sf::RenderWindow& window);

    void handleEvent(const sf::Event& e) override;
    void update(sf::Time dt) override;
    void render(sf::RenderTarget& target) override;

    bool shouldExit() const { return mExitRequested; }

private:
    void updateLayout();
    bool loadEncounterSprite();
    bool loadPlayerSprite();

private:
    sf::RenderWindow& mWindow;
    sf::View mBattleView;

    sf::RectangleShape mBackground;
    sf::Font mFont;
    bool mFontLoaded = false;

    bool mExitRequested = false;
    float mTimer = 0.f;

    // One-second encounter reveal before the full battle UI becomes interactive.
    bool mEncounterIntroActive = true;
    float mEncounterIntroTimer = 0.f;
    static constexpr float kEncounterIntroDuration = 1.0f;

    // Monster visuals.
    sf::Texture mDummyTexture;
    sf::Texture mEnemyTexture;
    sf::Texture mPlayerTexture;
    sf::Sprite mEnemySprite;
    sf::Sprite mPlayerSprite;
    bool mEnemySpriteLoaded = false;
    bool mPlayerSpriteLoaded = false;
    std::string mEnemySpritePath;

    sf::RectangleShape mEnemyPlatform;
    sf::RectangleShape mPlayerPlatform;

    // Debug-engine integration with dynamic challenge loading.
    Debug::Engine mDebugEngine;
    std::string mBattleMessage;
    bool mChallengeSolved = false;
    bool mSubmissionFailed = false;
    std::string mCurrentKeywordHint;

    static Debug::ChallengeLoader sChallengeLoader;
};
