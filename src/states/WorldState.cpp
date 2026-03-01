#include "WorldState.hpp"

#include <cmath>
#include <iostream>

namespace {
constexpr unsigned kTileSize = 48;
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

    const bool playerLoaded =
        mPlayerTex.loadFromFile("assets/player.png") ||
        mPlayerTex.loadFromFile("../assets/player.png");
    if (!playerLoaded) {
        std::cout << "Failed to load player.png\n";
    }

    mTrainerTexLoaded =
        mTrainerTex.loadFromFile("assets/npc.png") ||
        mTrainerTex.loadFromFile("../assets/npc.png");
    if (!mTrainerTexLoaded) {
        std::cout << "Failed to load npc.png\n";
    }

    if (playerLoaded) {
        mPlayer.setTexture(mPlayerTex, true);
    }

    if (mTrainerTexLoaded) {
        mTrainer.setTexture(mTrainerTex, true);
    }

    const float tileF = static_cast<float>(kTileSize);
    mPlayer.setPosition({100.f, 100.f});
    mTrainer.setPosition({300.f, 200.f});

    mTrainerFallback.setSize({tileF, tileF});
    mTrainerFallback.setFillColor(sf::Color::Red);
    mTrainerFallback.setPosition(mTrainer.getPosition());

    const auto ws = mWindow.getSize();
    mWorldView = sf::View(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(ws.x), static_cast<float>(ws.y)}
    ));
    mWorldView.setCenter(mPlayer.getPosition());

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
        testBounds.position = nextPos;

        if (!mMap.overlapsImpassable(testBounds)) {
            mPlayer.setPosition(nextPos);
        }
    }

    // Y-axis movement
    {
        const auto curPos = mPlayer.getPosition();
        const auto nextPos = sf::Vector2f{curPos.x, curPos.y + delta.y};
        auto testBounds = mPlayer.getGlobalBounds();
        testBounds.position = nextPos;

        if (!mMap.overlapsImpassable(testBounds)) {
            mPlayer.setPosition(nextPos);
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
    mWorldView.setCenter(mPlayer.getPosition());

    // Simple view clamping to map bounds
    const auto mapWidth = static_cast<float>(mMap.getWidth() * mMap.getTileSize());
    const auto mapHeight = static_cast<float>(mMap.getHeight() * mMap.getTileSize());
    const auto viewHalfWidth = mWorldView.getSize().x / 2.f;
    const auto viewHalfHeight = mWorldView.getSize().y / 2.f;

    auto center = mWorldView.getCenter();

    if (center.x - viewHalfWidth < 0.f)
        center.x = viewHalfWidth;
    if (center.x + viewHalfWidth > mapWidth)
        center.x = mapWidth - viewHalfWidth;

    if (center.y - viewHalfHeight < 0.f)
        center.y = viewHalfHeight;
    if (center.y + viewHalfHeight > mapHeight)
        center.y = mapHeight - viewHalfHeight;

    mWorldView.setCenter(center);
}

void WorldState::handleEvent(const sf::Event& e) {
    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::F1) {
            mDebugOpen = !mDebugOpen;
        }
    }
}

void WorldState::update(sf::Time dt) {
    const bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
    const bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
                      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
    const bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                      sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    const bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
                       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);

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
    target.draw(mPlayer);

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