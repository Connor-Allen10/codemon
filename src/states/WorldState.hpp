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
    bool isNearTrainer() const;
    void movePlayerWithCollision(sf::Vector2f delta);

private:
    sf::RenderWindow& mWindow;
    sf::View mWorldView;

    TileMap mMap;

    // Dummy texture so sf::Sprite can be constructed (SFML 3 requirement)
    sf::Texture mDummyTex;

    // Real textures (optional)
    sf::Texture mPlayerTex;
    sf::Texture mTrainerTex;

    bool mTrainerTexLoaded = false;

    // Sprites
    sf::Sprite mPlayer;
    sf::Sprite mTrainer;

    // Fallback visible trainer if texture fails
    sf::RectangleShape mTrainerFallback;

    // Movement / state
    float mSpeed = 220.f;
    bool mDebugOpen = false;
    bool mTrainerLocked = true;

    // simple overlay when debug UI is open
    sf::RectangleShape mOverlay;

    sf::Vector2f computeMovementInput(bool up, bool down, bool left, bool right);
    void applyMovement(sf::Vector2f move, sf::Time dt);
};