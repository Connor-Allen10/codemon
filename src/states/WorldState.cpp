/**
 * @file WorldState.cpp
 * @brief Implementation of the WorldState class.
 */

#include "WorldState.hpp"
#include "BattleState.hpp"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace {
const unsigned kTileSize = 48;  ///< Tile size in pixels

/**
 * @brief Scale a sprite to fit a tile size.
 * @param sprite The sprite to scale
 * @param tileSize Target size in pixels
 * 
 * Scales sprite to fit exactly within tileSize×tileSize,
 * preserving aspect ratio.
 */
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

sf::Vector2f WorldState::getPlayerCenter() const {
#if SFML_VERSION_MAJOR >= 3
    const auto pb = mPlayer.getGlobalBounds();
    return {pb.position.x + pb.size.x * 0.5f, pb.position.y + pb.size.y * 0.5f};
#else
    const auto pb = mPlayer.getGlobalBounds();
    return {pb.left + pb.width * 0.5f, pb.top + pb.height * 0.5f};
#endif
}

void WorldState::syncFallbackPositions() {
    mPlayerFallback.setPosition(mPlayer.getPosition());
    mTrainerFallback.setPosition(mTrainer.getPosition());
}

void WorldState::updateCamera() {
    mWorldView.setCenter(getPlayerCenter());

    const auto mapWidth = static_cast<float>(mMap.getWidth() * mMap.getTileSize());
    const auto mapHeight = static_cast<float>(mMap.getHeight() * mMap.getTileSize());
    const auto viewHalfWidth = mWorldView.getSize().x * 0.5f;
    const auto viewHalfHeight = mWorldView.getSize().y * 0.5f;

    auto center = mWorldView.getCenter();

    if (mapWidth <= 0.f || mapHeight <= 0.f) {
        return;
    }

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

WorldState::WorldState(sf::RenderWindow& window)
: mWindow(window)
, mPlayer(mDummyTex)
, mTrainer(mDummyTex) {
    const bool mapLoaded =
        mMap.loadFromCSV("assets/data/map01.csv", kTileSize) ||
        mMap.loadFromCSV("../assets/data/map01.csv", kTileSize);

    if (!mapLoaded) {
        std::cerr << "ERROR: Failed to load map file. Tried paths:\n"
                  << "  - " << std::filesystem::absolute("assets/data/map01.csv").string() << "\n"
                  << "  - " << std::filesystem::absolute("../assets/data/map01.csv").string() << "\n"
                  << "Current working directory: " << std::filesystem::current_path().string() << "\n";
    }

    mPlayerTexLoaded =
        mPlayerTex.loadFromFile("assets/player.png") ||
        mPlayerTex.loadFromFile("../assets/player.png");
    if (!mPlayerTexLoaded) {
        std::cerr << "WARNING: Failed to load player texture. Tried paths:\n"
                  << "  - " << std::filesystem::absolute("assets/player.png").string() << "\n"
                  << "  - " << std::filesystem::absolute("../assets/player.png").string() << "\n"
                  << "Using fallback blue rectangle.\n";
    }

    mTrainerTexLoaded =
        mTrainerTex.loadFromFile("assets/npc.png") ||
        mTrainerTex.loadFromFile("../assets/npc.png");
    if (!mTrainerTexLoaded) {
        std::cerr << "WARNING: Failed to load trainer texture. Tried paths:\n"
                  << "  - " << std::filesystem::absolute("assets/npc.png").string() << "\n"
                  << "  - " << std::filesystem::absolute("../assets/npc.png").string() << "\n"
                  << "Using fallback red rectangle.\n";
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

    mTrainerFallback.setSize({tileF, tileF});
    mTrainerFallback.setFillColor(sf::Color::Red);
    syncFallbackPositions();

    const auto ws = mWindow.getSize();
    mWorldView = sf::View(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(ws.x), static_cast<float>(ws.y)}
    ));
    updateCamera();

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
    auto currentCollisionBounds = [&]() {
        auto bounds = mPlayer.getGlobalBounds();
#if SFML_VERSION_MAJOR >= 3
        if (!mPlayerTexLoaded || bounds.size.x <= 0.f || bounds.size.y <= 0.f) {
            return mPlayerFallback.getGlobalBounds();
        }
#else
        if (!mPlayerTexLoaded || bounds.width <= 0.f || bounds.height <= 0.f) {
            return mPlayerFallback.getGlobalBounds();
        }
#endif
        return bounds;
    };

    // X-axis movement
    {
        const auto curPos = mPlayer.getPosition();
        const auto nextPos = sf::Vector2f{curPos.x + delta.x, curPos.y};
        auto testBounds = currentCollisionBounds();
    #if SFML_VERSION_MAJOR >= 3
        const auto size = testBounds.size;
        testBounds = sf::FloatRect(nextPos, size);
    #else
        const auto size = sf::Vector2f(testBounds.width, testBounds.height);
        testBounds = sf::FloatRect(nextPos.x, nextPos.y, size.x, size.y);
    #endif

        if (!mMap.overlapsImpassable(testBounds)) {
            mPlayer.setPosition(nextPos);
            syncFallbackPositions();
        }
    }

    // Y-axis movement
    {
        const auto curPos = mPlayer.getPosition();
        const auto nextPos = sf::Vector2f{curPos.x, curPos.y + delta.y};
        auto testBounds = currentCollisionBounds();
    #if SFML_VERSION_MAJOR >= 3
        const auto size = testBounds.size;
        testBounds = sf::FloatRect(nextPos, size);
    #else
        const auto size = sf::Vector2f(testBounds.width, testBounds.height);
        testBounds = sf::FloatRect(nextPos.x, nextPos.y, size.x, size.y);
    #endif

        if (!mMap.overlapsImpassable(testBounds)) {
            mPlayer.setPosition(nextPos);
            syncFallbackPositions();
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
    updateCamera();
}

void WorldState::handleEvent(const sf::Event& e) {
#if SFML_VERSION_MAJOR >= 3
    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::F1) {
            mDebugOpen = !mDebugOpen;
        }
    }
    if (const auto* resized = e.getIf<sf::Event::Resized>()) {
        mWorldView.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
        mOverlay.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
        updateCamera();
    }
#else
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::F1) {
            mDebugOpen = !mDebugOpen;
        }
    }
    if (e.type == sf::Event::Resized) {
        mWorldView.setSize({static_cast<float>(e.size.width), static_cast<float>(e.size.height)});
        mOverlay.setSize({static_cast<float>(e.size.width), static_cast<float>(e.size.height)});
        updateCamera();
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
    } else {
        // Keep camera clamping stable even when idle (e.g., after resize).
        updateCamera();
    }

    // Update encounter cooldown and check for random encounters
    if (mEncounterCooldown > 0.f) {
        mEncounterCooldown -= dt.asSeconds();
    } else {
        checkEncounter();
        // Always reset to base interval; checkEncounter will extend it if battle triggers
        mEncounterCooldown = kEncounterCheckInterval;
    }
}

void WorldState::checkEncounter() {
    // Check if player is on encounter-able tile
    const auto playerCenter = getPlayerCenter();
    if (mMap.isEncounterAt(playerCenter)) {
        // Random chance to trigger battle
        const float roll = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        if (roll < kEncounterChance) {
            std::cout << "[WorldState] Wild encounter! Transitioning to battle...\n";
            requestPush(std::make_unique<BattleState>(mWindow));
            // Set a long cooldown after battle trigger to prevent immediate re-encounter
            // when returning from battle while still on grass tile
            mEncounterCooldown = 3.0f;  // 3 second cooldown after battle
        }
    }
}

void WorldState::render(sf::RenderTarget& target) {
    target.setView(mWorldView);
    // Deterministic world draw order: map -> NPCs -> player -> UI overlay.
    target.draw(mMap);
    if (mTrainerTexLoaded) {
        target.draw(mTrainer);
    } else {
        target.draw(mTrainerFallback);
    }

    if (mPlayerTexLoaded) {
        target.draw(mPlayer);
    } else {
        target.draw(mPlayerFallback);
    }

    if (mDebugOpen) {
        target.setView(target.getDefaultView());
        target.draw(mOverlay);
    }
}