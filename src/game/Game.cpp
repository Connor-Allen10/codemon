#include "Game.hpp"

#include "../states/WorldState.hpp"
#include "../states/MainMenuState.hpp"

Game::Game()
: mWindow(sf::VideoMode({960, 540}), "Codemon")
, mStates()
{
    mWindow.setFramerateLimit(60);
    mStates.setRenderTarget(&mWindow);

    // Push the world first so it sits underneath the menu.
    mStates.push(std::make_unique<WorldState>(mWindow));

    // Push the main menu on top so the game starts there.
    mStates.push(std::make_unique<MainMenuState>(mWindow));
}

void Game::run()
{
    sf::Clock clock;

    while (mWindow.isOpen()) {
        sf::Time dt = clock.restart();

        #if SFML_VERSION_MAJOR >= 3
        while (auto eventOpt = mWindow.pollEvent()) {
            const sf::Event& event = *eventOpt;
            if (event.is<sf::Event::Closed>()) {
                mWindow.close();
            }
            mStates.handleEvent(event);
        }
        #else
        sf::Event event;
        while (mWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                mWindow.close();
            }
            mStates.handleEvent(event);
        }
        #endif

        mStates.update(dt);

        mWindow.clear();
        mStates.render();
        mWindow.display();
    }
}