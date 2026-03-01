#include "Game.hpp"
#include "../states/WorldState.hpp"

const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
: mWindow(sf::VideoMode({960, 540}), "Codemon")
, mStates()
{
    mWindow.setFramerateLimit(60);
    mStates.setRenderTarget(&mWindow);
    mStates.push(std::make_unique<WorldState>(mWindow));
}


void Game::run() {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (mWindow.isOpen()) {
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;

        while (timeSinceLastUpdate >= TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }

        render();
    }
}

void Game::processEvents() {
    sf::Event e;
    while (mWindow.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            mWindow.close();
            return;
        }
        mStates.handleEvent(e);
    }
}

void Game::update(sf::Time dt) {
    mStates.update(dt);
}

void Game::render() {
    mWindow.clear();
    mStates.render();
    mWindow.display();
}
