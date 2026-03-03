/**
 * @file Game.cpp
 * @brief Implementation of the main Game class.
 */

#include "Game.hpp"
#include "../states/WorldState.hpp"

// Fixed timestep: 16.67ms per frame = 60 FPS
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
        // Measure frame time
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;

        // Fixed timestep update: always 16.67ms per update
        // Multiple updates may happen if frame took too long
        while (timeSinceLastUpdate >= TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }

        // Render as fast as possible (limited by vsync/framerate)
        render();
    }
}

void Game::processEvents() {
#if SFML_VERSION_MAJOR >= 3
    while (auto e = mWindow.pollEvent()) {
        if (e->is<sf::Event::Closed>()) {
            mWindow.close();
            return;
        }
        mStates.handleEvent(*e);
    }
#else
    sf::Event e;
    while (mWindow.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            mWindow.close();
            return;
        }
        mStates.handleEvent(e);
    }
#endif
}

void Game::update(sf::Time dt) {
    mStates.update(dt);
}

void Game::render() {
    mWindow.clear();
    mStates.render();
    mWindow.display();
}
