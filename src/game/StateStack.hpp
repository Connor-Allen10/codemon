#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "game/State.hpp"

class StateStack {
public:
    void push(std::unique_ptr<State> state);
    void pop();

    void handleEvent(const sf::Event& e);
    void update(sf::Time dt);
    void render();

    void setRenderTarget(sf::RenderTarget* target); // optional, if you want later

private:
    std::vector<std::unique_ptr<State>> mStack;
    sf::RenderTarget* mTarget = nullptr;
};
