#pragma once

#include <SFML/Graphics.hpp>
#include "../game/State.hpp"

class MainMenuState : public State {
public:
    explicit MainMenuState(sf::RenderWindow& window);

    void handleEvent(const sf::Event& e) override;
    void update(sf::Time dt) override;
    void render(sf::RenderTarget& target) override;

private:
    sf::RenderWindow& mWindow;
    sf::View mMenuView;
    sf::RectangleShape mBackground;

    sf::Font mFont;
    bool mFontLoaded = false;

    float mTimer = 0.f;
};