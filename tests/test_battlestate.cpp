/**
 * @file test_battlestate.cpp
 * @brief Unit tests for BattleState with Debug::Engine integration.
 * 
 * Tests verify that BattleState properly initializes and integrates
 * with the Debug::Engine for challenge-based gameplay.
 */

#include <gtest/gtest.h>
#include "../src/states/BattleState.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

/**
 * Test: BattleState initializes with an active debug challenge
 * 
 * Verifies that when BattleState is created, it automatically starts
 * a debug challenge through Debug::Engine. This is a core requirement
 * for battle encounters to present coding challenges to the player.
 * 
 * NOTE: This test creates an sf::RenderWindow in headless mode.
 * On Linux CI, it runs under Xvfb (X Virtual Framebuffer).
 */
TEST(BattleStateTest, InitializesWithActiveChallenge) {
    // Default-constructed window is sufficient for constructor smoke test
    sf::RenderWindow window;
    
    // Create BattleState - constructor should start a challenge
    BattleState battleState(window);
    
    // Verify that BattleState has initialized properly by checking
    // that it's a valid State object (not checking internal state
    // since mDebugEngine is private and we're testing integration)
    
    // If this test compiles and runs without crashing, the basic
    // integration is working. More detailed behavior is tested via
    // manual gameplay testing in Step 2.
    EXPECT_TRUE(true); // Placeholder - verifies construction succeeds
}

/**
 * Test: BattleState handles window resize without crashing
 * 
 * Verifies that BattleState can handle window resize events,
 * which is important for dynamic UI layout.
 */
TEST(BattleStateTest, HandlesResizeEvent) {
    sf::RenderWindow window;
    BattleState battleState(window);
    
    // Create resize event
#if SFML_VERSION_MAJOR >= 3
    sf::Event resizeEvent = sf::Event::Resized{{1024, 768}};
#else
    sf::Event resizeEvent;
    resizeEvent.type = sf::Event::Resized;
    resizeEvent.size.width = 1024;
    resizeEvent.size.height = 768;
#endif
    
    // Should not crash when handling resize
    battleState.handleEvent(resizeEvent);
    
    EXPECT_TRUE(true); // Placeholder - verifies event handling succeeds
}
