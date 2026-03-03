/**
 * @file WorldState.hpp
 * @brief Main overworld gameplay state with player movement and encounters.
 * 
 * WorldState is the primary gameplay state where the player:
 * - Moves around the tile-based map
 * - Triggers random encounters on grass tiles
 * - Can interact with NPCs (future)
 * - Opens debug UI with F1
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "../game/State.hpp"
#include "../world/TileMap.hpp"

/**
 * @class WorldState
 * @brief Main overworld state with player movement and tile collision.
 * 
 * WorldState manages:
 * - Tile-based map rendering and collision
 * - Player and NPC sprite rendering with fallback rectangles
 * - Camera system that follows player and clamps to map bounds
 * - Random wild encounters (15% chance every 0.5s on grass)
 * - WASD/Arrow key movement with diagonal normalization
 * - F1 debug overlay toggle
 * - Window resize handling
 * 
 * The state automatically pushes BattleState when an encounter occurs
 * and resumes when the battle ends (BattleState pops itself).
 */
class WorldState : public State {
public:
    /**
     * @brief Construct the world state.
     * @param window Reference to the main game window
     * 
     * Loads map from CSV, attempts to load player/trainer textures,
     * initializes fallback sprites, and sets up camera view.
     */
    explicit WorldState(sf::RenderWindow& window);

    /**
     * @brief Handle input events.
     * @param e The SFML event to process
     * 
     * Handles:
     * - F1: Toggle debug overlay
     * - Window resize: Update view and overlay size
     */
    void handleEvent(const sf::Event& e) override;
    
    /**
     * @brief Update world logic.
     * @param dt Delta time since last update
     * 
     * Updates:
     * - Player movement based on WASD/Arrow keys
     * - Camera position (follows player, clamped to map)
     * - Encounter cooldown timer
     * - Random encounter checks on grass tiles
     */
    void update(sf::Time dt) override;
    
    /**
     * @brief Render world visuals.
     * @param target The render target to draw to
     * 
     * Draws in order: map → NPCs → player → debug overlay
     */
    void render(sf::RenderTarget& target) override;

private:
    /**
     * @brief Check if player is near trainer NPC.
     * @return true if within interaction range (96 pixels)
     * 
     * @note Not currently used for triggering interactions.
     */
    bool isNearTrainer() const;
    
    /**
     * @brief Move player with collision detection.
     * @param delta Movement vector in pixels
     * 
     * Performs swept collision on X and Y axes separately.
     * Uses fallback bounds if texture didn't load.
     */
    void movePlayerWithCollision(sf::Vector2f delta);
    
    /**
     * @brief Get the center point of the player sprite.
     * @return Center position in world coordinates
     * 
     * Used for camera following and encounter checks.
     */
    sf::Vector2f getPlayerCenter() const;
    
    /**
     * @brief Sync fallback rectangle positions with sprites.
     * 
     * Keeps fallback rectangles (used when textures fail to load)
     * in sync with sprite positions.
     */
    void syncFallbackPositions();
    
    /**
     * @brief Update camera to follow player.
     * 
     * Centers camera on player and clamps to map boundaries.
     * Handles maps smaller than viewport by centering map.
     */
    void updateCamera();
    
    /**
     * @brief Check for random wild encounters.
     * 
     * Called every 0.5s. If player is on grass tile, 15% chance
     * to push BattleState onto the stack.
     */
    void checkEncounter();

private:
    sf::RenderWindow& mWindow;           ///< Reference to main game window
    sf::View mWorldView;                 ///< Camera view following player

    TileMap mMap;                        ///< Tile-based map (collision + rendering)

    // Dummy texture for SFML 3 sprite construction requirement
    sf::Texture mDummyTex;

    // Real textures (may fail to load)
    sf::Texture mPlayerTex;              ///< Player sprite texture
    sf::Texture mTrainerTex;             ///< Trainer NPC texture

    bool mPlayerTexLoaded = false;       ///< Whether player texture loaded
    bool mTrainerTexLoaded = false;      ///< Whether trainer texture loaded

    // Sprites (use textures if loaded)
    sf::Sprite mPlayer;                  ///< Player sprite
    sf::Sprite mTrainer;                 ///< Trainer NPC sprite

    // Fallback rectangles (if textures fail)
    sf::RectangleShape mPlayerFallback;  ///< Blue rectangle for player
    sf::RectangleShape mTrainerFallback; ///< Red rectangle for trainer

    // Movement / state
    float mSpeed = 220.f;                ///< Player movement speed (pixels/second)
    bool mDebugOpen = false;             ///< Whether F1 debug overlay is shown
    bool mTrainerLocked = true;          ///< @unused Trainer interaction lock

    // Battle encounter tracking
    float mEncounterCooldown = 0.f;                    ///< Time until next encounter check
    static constexpr float kEncounterCheckInterval = 0.5f; ///< Check every 0.5s
    static constexpr float kEncounterChance = 0.15f;       ///< 15% chance on grass

    // Debug UI
    sf::RectangleShape mOverlay;         ///< Semi-transparent overlay for debug UI

    /**
     * @brief Compute movement vector from key states.
     * @return Normalized movement direction vector
     */
    sf::Vector2f computeMovementInput(bool up, bool down, bool left, bool right);
    
    /**
     * @brief Apply movement with collision and camera update.
     * @param move Normalized movement direction
     * @param dt Delta time
     */
    void applyMovement(sf::Vector2f move, sf::Time dt);
};