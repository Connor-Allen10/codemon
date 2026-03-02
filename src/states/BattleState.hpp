#pragma once

#include <SFML/Graphics.hpp>
#include "../game/State.hpp"

class BattleState : public State {
public:
    explicit BattleState(sf::RenderWindow& window);

    void handleEvent(const sf::Event& e) override;
    void update(sf::Time dt) override;
    void render(sf::RenderTarget& target) override;

    // Called by owning state to signal battle should end
    bool shouldExit() const { return mExitRequested; }

private:
    sf::RenderWindow& mWindow;
    sf::View mBattleView;

    // Battle UI elements
    sf::RectangleShape mBackground;
    sf::Font mFont;
    bool mFontLoaded = false;

    // Battle state
    bool mExitRequested = false;
    float mTimer = 0.f;
};
