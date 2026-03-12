/**
 * @file BattleState.hpp
 * @brief Battle encounter state with encounter intro, mon selection, and debug battle.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "../game/State.hpp"
#include "../debug/DebugChallenge.hpp"
#include "../debug/ChallengeLoader.hpp"

class Party;

class BattleState : public State {
public:
    explicit BattleState(sf::RenderWindow& window,
                         std::string preferredPlayerMonFile = "");
    BattleState(sf::RenderWindow& window,
                Party& party,
                std::string preferredPlayerMonFile = "");

    void handleEvent(const sf::Event& e) override;
    void update(sf::Time dt) override;
    void render(sf::RenderTarget& target) override;

    bool shouldExit() const { return mExitRequested; }

private:
    enum class Phase {
        EncounterIntro,
        PlayerSelect,
        DebugBattle
    };

    void updateViewLayout(sf::Vector2u size);
    void beginPlayerSelection();
    void confirmPlayerSelection();
    void refreshPlayerSprite();
    void moveSelection(int delta);
    void finalizeSubmissionResult();

    BattleState(sf::RenderWindow& window,
                Party* party,
                std::string preferredPlayerMonFile);

    sf::RenderWindow& mWindow;
    sf::View mBattleView;

    sf::RectangleShape mBackground;
    sf::RectangleShape mEnemyPlatform;
    sf::RectangleShape mPlayerPlatform;
    sf::Font mFont;
    bool mFontLoaded = false;

    sf::Texture mEnemyTexture;
    sf::Texture mPlayerTexture;
    sf::Sprite mEnemySprite;
    sf::Sprite mPlayerSprite;
    Party* mParty = nullptr;
    bool mEnemyTextureLoaded = false;
    bool mPlayerTextureLoaded = false;
    std::string mPreferredPlayerMonFile;
    std::string mPlayerMonFile;
    std::string mWildMonFile;

    std::vector<std::string> mSelectionMonFiles;
    std::vector<std::string> mSelectionDisplayNames;
    std::vector<sf::Texture> mSelectionTextures;
    std::vector<sf::Sprite> mSelectionSprites;
    std::vector<bool> mSelectionLoaded;
    std::size_t mSelectionIndex = 0;

    bool mExitRequested = false;
    float mTimer = 0.f;
    Phase mPhase = Phase::EncounterIntro;
    float mEncounterIntroTimer = 0.f;
    static constexpr float kEncounterIntroDuration = 2.0f;

    Debug::Engine mDebugEngine;
    std::string mBattleMessage;
    bool mChallengeSolved = false;
    bool mSubmissionFailed = false;
    std::string mCurrentKeywordHint;

    static Debug::ChallengeLoader sChallengeLoader;
};
