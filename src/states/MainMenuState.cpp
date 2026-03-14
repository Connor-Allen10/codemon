#include "MainMenuState.hpp"

#include <cstdlib>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

// Shared runtime toggle for noisy state-transition logs.
// Default: quiet. Enable with CODEMON_VERBOSE_STATE_LOGS=1.
bool isStateLoggingEnabled() {
    const char* env = std::getenv("CODEMON_VERBOSE_STATE_LOGS");
    if (env == nullptr) {
        return false;
    }

    const std::string value = env;
    return value == "1" || value == "true" || value == "TRUE" || value == "on" || value == "ON";
}

} // namespace

MainMenuState::MainMenuState(sf::RenderWindow& window)
: mWindow(window)
, mBackgroundSprite(mBackgroundTexture)
{
    mBackground.setFillColor(sf::Color(10, 10, 30));

    const std::vector<std::string> backgroundCandidates = {
        "src/assets/MainMenu.png",
        "assets/MainMenu.png",
        "./src/assets/MainMenu.png",
        "./assets/MainMenu.png"
    };

    for (const auto& path : backgroundCandidates) {
        // Probe candidate paths quietly; this prevents noisy load failures
        // when running from a different working directory.
        std::error_code ec;
        if (!std::filesystem::exists(path, ec) || ec) {
            continue;
        }
        if (mBackgroundTexture.loadFromFile(path)) {
            mBackgroundTexture.setSmooth(true);
            mBackgroundSprite.setTexture(mBackgroundTexture, true);
            mBackgroundTextureLoaded = true;
            break;
        }
    }

    if (!mBackgroundTextureLoaded) {
        std::cerr << "WARNING: Failed to load main menu background image. Using solid color fallback.\n";
    }

    const std::vector<std::string> fontCandidates = {
        "assets/fonts/arial.ttf",
        "./assets/fonts/arial.ttf",
        "src/assets/fonts/arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };

    for (const auto& path : fontCandidates) {
        std::error_code ec;
        if (!std::filesystem::exists(path, ec) || ec) {
            continue;
        }

#if SFML_VERSION_MAJOR >= 3
        if (mFont.openFromFile(path)) {
#else
        if (mFont.loadFromFile(path)) {
#endif
            mFontLoaded = true;
            break;
        }
    }

    if (!mFontLoaded) {
        std::cerr << "WARNING: Failed to load menu font. Text will not display.\n";
    }

    // Startup trace is useful for debugging state flow, but too noisy for
    // normal gameplay/test output, so it's behind the runtime toggle.
    if (isStateLoggingEnabled()) {
        std::cout << "[MainMenuState] Initialized\n";
    }
}

void MainMenuState::handleEvent(const sf::Event& e)
{
#if SFML_VERSION_MAJOR >= 3
    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter ||
            keyPressed->code == sf::Keyboard::Key::Space) {
            requestPop();
            if (isStateLoggingEnabled()) {
                std::cout << "[MainMenuState] Start requested\n";
            }
        }

        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            mWindow.close();
        }
    }
#else
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Enter ||
            e.key.code == sf::Keyboard::Space) {
            requestPop();
            if (isStateLoggingEnabled()) {
                std::cout << "[MainMenuState] Start requested\n";
            }
        }

        if (e.key.code == sf::Keyboard::Escape) {
            mWindow.close();
        }
    }
#endif
}

void MainMenuState::update(sf::Time dt)
{
    mTimer += dt.asSeconds();
}

void MainMenuState::render(sf::RenderTarget& target)
{
    const sf::Vector2u size = target.getSize();
    const float width  = static_cast<float>(size.x);
    const float height = static_cast<float>(size.y);

    // Create a fresh fullscreen view every frame based on the actual target size
    sf::View menuView(sf::FloatRect({0.f, 0.f}, {width, height}));
    target.setView(menuView);

    if (mBackgroundTextureLoaded) {
        const sf::Vector2u texSize = mBackgroundTexture.getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            mBackgroundSprite.setPosition({0.f, 0.f});
            mBackgroundSprite.setScale({
                width / static_cast<float>(texSize.x),
                height / static_cast<float>(texSize.y)
            });
            target.draw(mBackgroundSprite);
        }
    } else {
        // Fill the whole screen
        mBackground.setPosition({0.f, 0.f});
        mBackground.setSize({width, height});
        target.draw(mBackground);
    }

    if (mFontLoaded) {
#if SFML_VERSION_MAJOR >= 3
        // sf::Text title(mFont, "CODEMON", 72);
        sf::Text subtitle(mFont, "Press ENTER to Start", 30);
#else
        sf::Text title("CODEMON", mFont, 72);
        sf::Text subtitle("Press ENTER to Start", mFont, 30);
#endif

        // title.setFillColor(sf::Color::White);

        const float pulse = 0.5f + 0.5f * std::sin(mTimer * 2.5f);
        const auto alpha = static_cast<std::uint8_t>(140 + 115 * pulse);
        subtitle.setFillColor(sf::Color(255, 255, 255, alpha));

#if SFML_VERSION_MAJOR >= 3
        /*
         const auto titleBounds = title.getLocalBounds();
        title.setOrigin({
            titleBounds.position.x + titleBounds.size.x * 0.5f,
            titleBounds.position.y + titleBounds.size.y * 0.5f
        });
        */

        const auto subtitleBounds = subtitle.getLocalBounds();
        subtitle.setOrigin({
            subtitleBounds.position.x + subtitleBounds.size.x * 0.5f,
            subtitleBounds.position.y + subtitleBounds.size.y * 0.5f
        });
#else
        const auto titleBounds = title.getLocalBounds();
        title.setOrigin({
            titleBounds.left + titleBounds.width * 0.5f,
            titleBounds.top + titleBounds.height * 0.5f
        });

        const auto subtitleBounds = subtitle.getLocalBounds();
        subtitle.setOrigin({
            subtitleBounds.left + subtitleBounds.width * 0.5f,
            subtitleBounds.top + subtitleBounds.height * 0.5f
        });
#endif

        const float centerX = width * 0.5f;
        const float centerY = height * 0.5f;

        // title.setPosition({centerX, centerY - 60.f});
        subtitle.setPosition({centerX, centerY + 20.f});

        // target.draw(title);
        target.draw(subtitle);
    }
}