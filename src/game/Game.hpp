#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include "game/StateStack.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time dt);
    void render();

private:
    sf::RenderWindow mWindow;
    StateStack mStates;
    static constexpr sf::Time TimePerFrame = sf::seconds(1.f / 60.f);
};
