#pragma once
#include <SFML/Graphics.hpp>

class State {
public:
    virtual ~State() = default;
    virtual void handleEvent(const sf::Event& e) = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;
};
