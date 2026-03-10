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

        while (auto eventOpt = mWindow.pollEvent()) {
            const sf::Event& event = *eventOpt;

#if SFML_VERSION_MAJOR >= 3
            if (event.is<sf::Event::Closed>()) {
                mWindow.close();
            }
#else
            if (event.type == sf::Event::Closed) {
                mWindow.close();
            }
#endif

            mStates.handleEvent(event);
        }

        mStates.update(dt);

        mWindow.clear();
        mStates.render();
        mWindow.display();
    }
}