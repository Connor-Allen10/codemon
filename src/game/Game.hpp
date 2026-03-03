/**
 * @file Game.hpp
 * @brief Main game class that manages the game loop and window.
 * 
 * The Game class owns the SFML window and StateStack, and runs the
 * main game loop with fixed timestep updates and variable rendering.
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include "StateStack.hpp"

/**
 * @class Game
 * @brief Main game controller that runs the game loop.
 * 
 * The Game class:
 * - Creates and manages the SFML render window
 * - Owns the StateStack and initializes with WorldState
 * - Runs the fixed-timestep game loop (60 FPS)
 * - Processes events, updates logic, and renders each frame
 * 
 * The game loop uses a fixed timestep for updates (16.67ms) to ensure
 * consistent physics/logic, while rendering happens as fast as possible.
 */
class Game {
public:
    /**
     * @brief Construct the game and initialize the window and states.
     * 
     * Creates a 960x540 window, initializes the StateStack with WorldState,
     * and sets the framerate limit to 60 FPS.
     */
    Game();
    
    /**
     * @brief Run the main game loop.
     * 
     * Runs until the window is closed. Uses fixed timestep for updates
     * (16.67ms per frame = 60 FPS) and renders as fast as possible.
     */
    void run();

private:
    /**
     * @brief Process window and input events.
     * 
     * Polls SFML events and forwards them to the active state.
     * Handles window close events.
     */
    void processEvents();
    
    /**
     * @brief Update game logic with fixed timestep.
     * @param dt Delta time (always TimePerFrame = 16.67ms)
     * 
     * Updates the active state with a fixed timestep for consistent
     * physics and game logic.
     */
    void update(sf::Time dt);
    
    /**
     * @brief Render the current frame.
     * 
     * Clears the window, renders the active state, and displays.
     */
    void render();

private:
    sf::RenderWindow mWindow;                    ///< Main game window (960x540)
    StateStack mStates;                          ///< State manager (owns all states)
    static const sf::Time TimePerFrame;          ///< Fixed timestep (16.67ms = 60 FPS)
};
