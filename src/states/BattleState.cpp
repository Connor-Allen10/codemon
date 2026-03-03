/**
 * @file BattleState.cpp
 * @brief Implementation of the BattleState class.
 */

#include "BattleState.hpp"

#include <cmath>
#include <filesystem>
#include <iostream>

BattleState::BattleState(sf::RenderWindow& window)
: mWindow(window) {
    // Initialize view to match window size
    const auto ws = mWindow.getSize();
    mBattleView = sf::View(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(ws.x), static_cast<float>(ws.y)}
    ));

    // Setup battle background (dark purple)
    mBackground.setPosition({0.f, 0.f});
    mBackground.setSize({static_cast<float>(ws.x), static_cast<float>(ws.y)});
    mBackground.setFillColor(sf::Color(64, 0, 128));

    // Try to load font (SFML 3.0 uses openFromFile, 2.x uses loadFromFile)
#if SFML_VERSION_MAJOR >= 3
    mFontLoaded = mFont.openFromFile("assets/fonts/default.ttf") ||
                  mFont.openFromFile("../assets/fonts/default.ttf") ||
                  mFont.openFromFile("/System/Library/Fonts/Helvetica.ttc") ||
                  mFont.openFromFile("C:/Windows/Fonts/arial.ttf") ||
                  mFont.openFromFile("C:/Windows/Fonts/calibri.ttf");
#else
    mFontLoaded = mFont.loadFromFile("assets/fonts/default.ttf") ||
                  mFont.loadFromFile("../assets/fonts/default.ttf") ||
                  mFont.loadFromFile("/System/Library/Fonts/Helvetica.ttc") ||
                  mFont.loadFromFile("C:/Windows/Fonts/arial.ttf") ||
                  mFont.loadFromFile("C:/Windows/Fonts/calibri.ttf");
#endif

    if (!mFontLoaded) {
        std::cerr << "WARNING: Failed to load battle font. Text will not display.\n";
    }

    std::cout << "[BattleState] Initialized\n";
}

void BattleState::handleEvent(const sf::Event& e) {
#if SFML_VERSION_MAJOR >= 3
    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            mExitRequested = true;
            requestPop(); // Request to be removed from state stack
            std::cout << "[BattleState] Exit requested\n";
        }
    }
    if (const auto* resized = e.getIf<sf::Event::Resized>()) {
        mBattleView.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
        mBackground.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
    }
#else
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Escape) {
            mExitRequested = true;
            requestPop(); // Request to be removed from state stack
            std::cout << "[BattleState] Exit requested\n";
        }
    }
    if (e.type == sf::Event::Resized) {
        mBattleView.setSize({static_cast<float>(e.size.width), static_cast<float>(e.size.height)});
        mBackground.setSize({static_cast<float>(e.size.width), static_cast<float>(e.size.height)});
    }
#endif
}

void BattleState::update(sf::Time dt) {
    mTimer += dt.asSeconds();
}

void BattleState::render(sf::RenderTarget& target) {
    target.setView(mBattleView);
    target.draw(mBackground);
    
    // Draw text with pulse animation (if font loaded)
    if (mFontLoaded) {
        // SFML 3.0 vs 2.x have different Text constructor parameter order
#if SFML_VERSION_MAJOR >= 3
        sf::Text text(mFont, "BATTLE! Press ESC to exit", 36);
#else
        sf::Text text("BATTLE! Press ESC to exit", mFont, 36);
#endif
        text.setFillColor(sf::Color::White);
        
        // Center text on screen
#if SFML_VERSION_MAJOR >= 3
        const auto bounds = text.getLocalBounds();
        text.setOrigin({bounds.size.x * 0.5f, bounds.size.y * 0.5f});
#else
        const auto bounds = text.getLocalBounds();
        text.setOrigin({bounds.width * 0.5f, bounds.height * 0.5f});
#endif
        text.setPosition({static_cast<float>(mWindow.getSize().x) * 0.5f, 
                          static_cast<float>(mWindow.getSize().y) * 0.5f});
        
        // Apply pulse effect to alpha channel (fade in/out)
        const float pulse = 0.5f + 0.5f * std::sin(mTimer * 3.f);
        const auto alpha = static_cast<std::uint8_t>(128 + 127 * pulse);
        text.setFillColor(sf::Color(255, 255, 255, alpha));
        
        target.draw(text);
    }
}
