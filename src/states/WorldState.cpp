#include "WorldState.hpp"

#include <cmath>
#include <iostream>

namespace {
constexpr unsigned kTileSize = 48;

void scaleSpriteToTile(sf::Sprite& sprite, float tileSize) {
    const auto b = sprite.getLocalBounds();
#if SFML_VERSION_MAJOR >= 3
    const float w = b.size.x;
    const float h = b.size.y;
#else
    const float w = b.width;
    const float h = b.height;
#endif
    if (w > 0.f && h > 0.f) {
        sprite.setScale({tileSize / w, tileSize / h});
    }
}
}

WorldState::WorldState(sf::RenderWindow& window)
: mWindow(window)
, mPlayer(mDummyTex)
, mTrainer(mDummyTex) {
    const bool mapLoaded =
        mMap.loadFromCSV("assets/data/map01.csv", kTileSize) ||
        mMap.loadFromCSV("../assets/data/map01.csv", kTileSize);

    if (!mapLoaded) {
        std::cout << "Failed to load assets/data/map01.csv\n";
    }

    mPlayerTexLoaded =
        mPlayerTex.loadFromFile("assets/player.png") ||
        mPlayerTex.loadFromFile("../assets/player.png");
    if (!mPlayerTexLoaded) {
        std::cout << "Failed to load player.png\n";
    }

    mTrainerTexLoaded =
        mTrainerTex.loadFromFile("assets/npc.png") ||
        mTrainerTex.loadFromFile("../assets/npc.png");
    if (!mTrainerTexLoaded) {
        std::cout << "Failed to load npc.png\n";
    }

    if (mPlayerTexLoaded) {
        mPlayer.setTexture(mPlayerTex, true);
        scaleSpriteToTile(mPlayer, static_cast<float>(kTileSize));
    }

    if (mTrainerTexLoaded) {
        mTrainer.setTexture(mTrainerTex, true);
        scaleSpriteToTile(mTrainer, static_cast<float>(kTileSize));
    }

    const float tileF = static_cast<float>(kTileSize);
    mPlayer.setPosition({100.f, 100.f});
    mTrainer.setPosition({300.f, 200.f});

    mPlayerFallback.setSize({tileF, tileF});
    mPlayerFallback.setFillColor(sf::Color::Blue);
    mPlayerFallback.setPosition(mPlayer.getPosition());

    mTrainerFallback.setSize({tileF, tileF});
    mTrainerFallback.setFillColor(sf::Color::Red);
    mTrainerFallback.setPosition(mTrainer.getPosition());

    const auto ws = mWindow.getSize();
    mWorldView = sf::View(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(ws.x), static_cast<float>(ws.y)}
    ));
#if SFML_VERSION_MAJOR >= 3
    const auto pb = mPlayer.getGlobalBounds();
    mWorldView.setCenter({pb.position.x + pb.size.x * 0.5f, pb.position.y + pb.size.y * 0.5f});
#else
    const auto pb = mPlayer.getGlobalBounds();
    mWorldView.setCenter({pb.left + pb.width * 0.5f, pb.top + pb.height * 0.5f});
#endif

    mOverlay.setPosition({0.f, 0.f});
    mOverlay.setSize({static_cast<float>(ws.x), static_cast<float>(ws.y)});
    mOverlay.setFillColor(sf::Color(0, 0, 0, 120));
}

bool WorldState::isNearTrainer() const {
    const auto playerPos = mPlayer.getPosition();
    const auto trainerPos = mTrainer.getPosition();
    const auto dx = playerPos.x - trainerPos.x;
    const auto dy = playerPos.y - trainerPos.y;
    const auto distSq = dx * dx + dy * dy;
    const float threshold = 96.f;
    return distSq < threshold * threshold;
}

void WorldState::movePlayerWithCollision(sf::Vector2f delta) {
    // X-axis movement
    {
        const auto curPos = mPlayer.getPosition();
        const auto nextPos = sf::Vector2f{curPos.x + delta.x, curPos.y};
        auto testBounds = mPlayer.getGlobalBounds();
    #if SFML_VERSION_MAJOR >= 3
        const auto size = testBounds.size;
        testBounds = sf::FloatRect(nextPos, size);
    #else
        const auto size = sf::Vector2f(testBounds.width, testBounds.height);
        testBounds = sf::FloatRect(nextPos.x, nextPos.y, size.x, size.y);
    #endif

        if (!mMap.overlapsImpassable(testBounds)) {
            mPlayer.setPosition(nextPos);
            mPlayerFallback.setPosition(nextPos);
        }
    }

    // Y-axis movement
    {
        const auto curPos = mPlayer.getPosition();
        const auto nextPos = sf::Vector2f{curPos.x, curPos.y + delta.y};
        auto testBounds = mPlayer.getGlobalBounds();
    #if SFML_VERSION_MAJOR >= 3
        const auto size = testBounds.size;
        testBounds = sf::FloatRect(nextPos, size);
    #else
        const auto size = sf::Vector2f(testBounds.width, testBounds.height);
        testBounds = sf::FloatRect(nextPos.x, nextPos.y, size.x, size.y);
    #endif

        if (!mMap.overlapsImpassable(testBounds)) {
            mPlayer.setPosition(nextPos);
            mPlayerFallback.setPosition(nextPos);
        }
    }
}

sf::Vector2f WorldState::computeMovementInput(bool up, bool down, bool left, bool right) {
    auto movement = sf::Vector2f{0.f, 0.f};

    if (up) {
        movement.y = -1.f;
    } else if (down) {
        movement.y = 1.f;
    }

    if (left) {
        movement.x = -1.f;
    } else if (right) {
        movement.x = 1.f;
    }

    return movement;
}

void WorldState::applyMovement(sf::Vector2f move, sf::Time dt) {
    movePlayerWithCollision(move * (mSpeed * dt.asSeconds()));
#if SFML_VERSION_MAJOR >= 3
    const auto pb = mPlayer.getGlobalBounds();
    mWorldView.setCenter({pb.position.x + pb.size.x * 0.5f, pb.position.y + pb.size.y * 0.5f});
#else
    const auto pb = mPlayer.getGlobalBounds();
    mWorldView.setCenter({pb.left + pb.width * 0.5f, pb.top + pb.height * 0.5f});
#endif

    // Simple view clamping to map bounds
    const auto mapWidth = static_cast<float>(mMap.getWidth() * mMap.getTileSize());
    const auto mapHeight = static_cast<float>(mMap.getHeight() * mMap.getTileSize());
    const auto viewHalfWidth = mWorldView.getSize().x / 2.f;
    const auto viewHalfHeight = mWorldView.getSize().y / 2.f;

    auto center = mWorldView.getCenter();

    // If map failed to load, don't clamp to invalid dimensions.
    if (mapWidth <= 0.f || mapHeight <= 0.f) {
        return;
    }

    // If map is smaller than the window, center view on map.
    if (mapWidth <= mWorldView.getSize().x) {
        center.x = mapWidth * 0.5f;
    } else {
        if (center.x - viewHalfWidth < 0.f)
            center.x = viewHalfWidth;
        if (center.x + viewHalfWidth > mapWidth)
            center.x = mapWidth - viewHalfWidth;
    }

    if (mapHeight <= mWorldView.getSize().y) {
        center.y = mapHeight * 0.5f;
    } else {
        if (center.y - viewHalfHeight < 0.f)
            center.y = viewHalfHeight;
        if (center.y + viewHalfHeight > mapHeight)
            center.y = mapHeight - viewHalfHeight;
    }

    mWorldView.setCenter(center);
}

void WorldState::handleEvent(const sf::Event& e) {
#if SFML_VERSION_MAJOR >= 3
    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::F1) {
            mDebugOpen = !mDebugOpen;
        }
    }
#else
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::F1) {
            mDebugOpen = !mDebugOpen;
        }
    }
#endif
}

void WorldState::update(sf::Time dt) {
#if SFML_VERSION_MAJOR >= 3
    const bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
    const bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
                      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
    const bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    const bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
#else
    const bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    const bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
                      sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    const bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
                      sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    const bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
                       sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
#endif

    auto move = computeMovementInput(up, down, left, right);

    if (move.x != 0.f || move.y != 0.f) {
        // Normalize diagonal movement
        const auto mag = std::sqrt(move.x * move.x + move.y * move.y);
        if (mag > 0.f) {
            move /= mag;
        }
        applyMovement(move, dt);
    }
}

void WorldState::render(sf::RenderTarget& target) {
    target.setView(mWorldView);
    target.draw(mMap);
    if (mPlayerTexLoaded) {
        target.draw(mPlayer);
    } else {
        target.draw(mPlayerFallback);
    }

    if (mTrainerTexLoaded) {
        target.draw(mTrainer);
    } else {
        target.draw(mTrainerFallback);
    }

    if (mDebugOpen) {
        target.setView(target.getDefaultView());
        target.draw(mOverlay);
    }
}