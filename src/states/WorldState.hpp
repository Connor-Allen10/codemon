/**
 * @file WorldState.hpp
 * @brief Main overworld gameplay state with player movement and encounters.
 */

#pragma once

#include <SFML/Graphics.hpp>
#include "../game/State.hpp"
#include "../world/TileMap.hpp"

class WorldState : public State {
public:
    explicit WorldState(sf::RenderWindow& window);

    void handleEvent(const sf::Event& e) override;
    void update(sf::Time dt) override;
    void render(sf::RenderTarget& target) override;

private:
    enum class Facing { South = 0, North, East, West };

    bool isNearTrainer() const;
    void movePlayerWithCollision(sf::Vector2f delta);
    sf::Vector2f getPlayerCenter() const;
    void syncFallbackPositions();
    void updateCamera();
    void checkEncounter();

    sf::Vector2f computeMovementInput(bool up, bool down, bool left, bool right);
    void applyMovement(sf::Vector2f move, sf::Time dt);

    sf::Texture& textureForFacing(Facing facing);
    const sf::Texture& textureForFacing(Facing facing) const;
    sf::IntRect frameRect2x2(int frame) const;
    sf::Vector2f frameOffset(Facing facing, int frame) const;
    void updatePlayerVisual();
    sf::FloatRect playerCollisionRectAt(sf::Vector2f topLeft) const;

private:
    sf::RenderWindow& mWindow;
    sf::View mWorldView;

    TileMap mMap;

    sf::Texture mDummyTex;

    sf::Texture mPlayerTexSouth;
    sf::Texture mPlayerTexNorth;
    sf::Texture mPlayerTexEast;
    sf::Texture mPlayerTexWest;
    sf::Texture mTrainerTex;

    bool mPlayerTexLoaded = false;
    bool mTrainerTexLoaded = false;

    sf::Sprite mPlayer;
    sf::Sprite mTrainer;

    sf::RectangleShape mPlayerFallback;
    sf::RectangleShape mTrainerFallback;

    sf::Vector2f mPlayerTopLeft{100.f, 100.f};
    float mSpeed = 220.f;
    bool mDebugOpen = false;
    bool mTrainerLocked = true;

    Facing mFacing = Facing::South;
    int mAnimIndex = 0;
    float mAnimTimer = 0.f;

    float mEncounterCooldown = 0.f;
    static constexpr float kEncounterCheckInterval = 0.5f;
    static constexpr float kEncounterChance = 0.15f;

    sf::RectangleShape mOverlay;
};
