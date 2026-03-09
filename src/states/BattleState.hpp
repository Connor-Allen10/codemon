/**
 * @file BattleState.hpp
 * @brief Battle encounter state with simple placeholder UI.
 * 
 * This state is pushed onto the stack when a wild encounter occurs.
 * Currently displays a purple background with text. Press ESC to exit
 * and return to the world.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "../game/State.hpp"
#include "../debug/DebugChallenge.hpp"
#include "../debug/ChallengeLoader.hpp"

/**
 * @class BattleState
 * @brief State for handling battle encounters.
 * 
 * BattleState is pushed onto the StateStack when:
 * - Player walks on grass tiles (random 15% encounter chance)
 * - Player interacts with trainer NPCs (not yet implemented)
 * 
 * Current implementation:
 * - Displays purple background (64, 0, 128)
 * - Shows "BATTLE! Press ESC to exit" text
 * - ESC key pops state and returns to world
 * 
 * Future enhancements:
 * - Turn-based combat system
 * - Move selection UI
 * - Enemy/player monster display
 * - Battle result handling (win/lose/flee)
 */
class BattleState : public State {
public:
    /**
     * @brief Construct a new battle state.
     * @param window Reference to the main game window
     * 
     * Initializes battle view, background, and attempts to load a font
     * for text display. Falls back gracefully if font loading fails.
     */
    explicit BattleState(sf::RenderWindow& window);

    /**
     * @brief Handle input events during battle.
     * @param e The SFML event to process
     * 
     * Currently handles:
     * - ESC key: request pop to return to world
     * - Window resize: adjust view and background size
     */
    void handleEvent(const sf::Event& e) override;
    
    /**
     * @brief Update battle logic.
     * @param dt Delta time since last update
     * 
     * Currently just increments timer for text pulse animation.
     * Future: handle turn-based battle logic.
     */
    void update(sf::Time dt) override;
    
    /**
     * @brief Render battle visuals.
     * @param target The render target to draw to
     * 
     * Draws purple background and animated "BATTLE!" text.
     */
    void render(sf::RenderTarget& target) override;

    /**
     * @brief Check if battle has requested exit.
     * @return true if ESC was pressed
     * 
     * @deprecated Not currently used. State pops itself via requestPop().
     */
    bool shouldExit() const { return mExitRequested; }

private:
    sf::RenderWindow& mWindow;           ///< Reference to main game window
    sf::View mBattleView;                ///< View for battle rendering

    // Battle UI elements
    sf::RectangleShape mBackground;      ///< Purple background (64, 0, 128)
    sf::Font mFont;                      ///< Font for text display
    bool mFontLoaded = false;            ///< Whether font loaded successfully

    // Battle state
    bool mExitRequested = false;         ///< Whether ESC was pressed
    float mTimer = 0.f;                  ///< Timer for animations (pulse effect)

    // Debug-engine integration with dynamic challenge loading
    Debug::Engine mDebugEngine;
    std::string mBattleMessage;
    bool mChallengeSolved = false;
    bool mSubmissionFailed = false;  ///< True if last submission was incorrect
    std::string mCurrentKeywordHint;
    
    // Static challenge pool shared across all battle instances
    static Debug::ChallengeLoader sChallengeLoader;
};
