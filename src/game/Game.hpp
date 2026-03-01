#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include "StateStack.hpp"

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
    static const sf::Time TimePerFrame;
};
