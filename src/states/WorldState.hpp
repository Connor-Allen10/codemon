#pragma once
#include <SFML/Graphics.hpp>
#include "../game/State.hpp"

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

    sf::RectangleShape mPlayer;
    sf::RectangleShape mObstacle;

    bool mObstacleLocked = true;
    bool mDebugOpen = false;

    // UI overlay
    sf::RectangleShape mOverlay;
};
