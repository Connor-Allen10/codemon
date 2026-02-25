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
    bool isNearObstacle() const;

private:
    sf::RenderWindow& mWindow;

    // Camera view that follows the player
    sf::View mWorldView;

    sf::Texture playerTexture;
    sf::Texture playerTexture2;
    sf::Texture playerReverse;

    sf::Texture obstacleTexture;
    sf::Texture obstacleTexture2;

    sf::Sprite mPlayer; 
    sf::Sprite mObstacle;

    // Tile map (background)
    TileMap mMap;

    bool mObstacleLocked = true;
    bool mDebugOpen = false;

    // UI overlay
    sf::RectangleShape mOverlay;
};