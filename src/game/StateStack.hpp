#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "State.hpp"

class StateStack {
public:
    void push(std::unique_ptr<State> state);
    void pop();

    void handleEvent(const sf::Event& e);
    void update(sf::Time dt);
    void render();

    void setRenderTarget(sf::RenderTarget* target);

    // Check if stack is empty
    bool isEmpty() const { return mStack.empty(); }
    size_t size() const { return mStack.size(); }

private:
    void processPendingActions();

private:
    std::vector<std::unique_ptr<State>> mStack;
    sf::RenderTarget* mTarget = nullptr;
};
