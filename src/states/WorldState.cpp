#include "WorldState.hpp"
#include <cmath>
#include <iostream>
#include <filesystem>

static float length(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

WorldState::WorldState(sf::RenderWindow& window)
: mWindow(window),
  mPlayer(playerTexture),
  mObstacle(obstacleTexture)
{
    // =================
    // Load all textures
    // =================

    // NOTE: CMake copies the `assets/` folder into the build directory.
    // When you run the game from the build folder (default for most IDEs),
    // these paths should be relative to the build working directory.
    if (!playerTexture.loadFromFile("assets/player.png")) {
        std::cerr << "Failed to load player.png\n";
    }
    if (!playerTexture2.loadFromFile("assets/player_state2.png")) {
        std::cerr << "Failed to load player_state2.png\n";
    }
    if (!playerReverse.loadFromFile("assets/player_reverse.png")) {
        std::cerr << "Failed to load player_reverse.png\n";
    }

    if (!obstacleTexture.loadFromFile("assets/npc.png")) {
        std::cerr << "Failed to load npc.png\n";
    }
    if (!obstacleTexture2.loadFromFile("assets/npc_state2.png")) {
        std::cerr << "Failed to load npc_state2.png\n";
    }

    // =============
    // Load the map
    // =============
    // This draws a simple colored tile grid based on the CSV values in assets/data/map01.csv
    // (0 = Path, 1 = Grass, 2 = Wall)
    if (!mMap.loadFromCSV("assets/data/map01.csv", 48)) {
        std::cerr << "Failed to load assets/data/map01.csv\n";
    }

    mPlayer = sf::Sprite(playerTexture);
    mObstacle = sf::Sprite(obstacleTexture);
    
    // Initialize Sprite texture and position
    mPlayer.setTexture(playerTexture);
    mPlayer.setPosition({100.f, 100.f});

    mObstacle.setTexture(obstacleTexture);
    mObstacle.setPosition({300.f, 100.f});


    // Debug overlay
    mOverlay.setSize(sf::Vector2f(mWindow.getSize()));
    mOverlay.setFillColor(sf::Color(0, 0, 0, 160));

    // Camera setup: start as default view and center on player
    mWorldView = mWindow.getDefaultView();
    sf::FloatRect bounds = mPlayer.getGlobalBounds();
    sf::Vector2f playerCenter = 
    {
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f
    };
    mWorldView.setCenter(playerCenter);
}

bool WorldState::isNearObstacle() const {
    sf::Vector2f p = mPlayer.getPosition();
    sf::Vector2f o = mObstacle.getPosition();
    // crude proximity check
    return length((p + sf::Vector2f(50.f, 50.f)) - (o + sf::Vector2f(50.f, 50.f))) < 120.f;
}

void WorldState::handleEvent(const sf::Event& e) {
    if (e.is<sf::Event::KeyPressed>()) {
        const auto* kp = e.getIf<sf::Event::KeyPressed>();
        if (!kp) return;

        if (kp->code == sf::Keyboard::Key::E) {
            if (mObstacleLocked && isNearObstacle()) {
                mDebugOpen = !mDebugOpen;
            }
        }
        if (kp->code == sf::Keyboard::Key::Enter) {
            if (mDebugOpen) {
                mObstacleLocked = false;
                mDebugOpen = false;
                mObstacle.setTexture(obstacleTexture2);
            }
        }
        if (kp->code == sf::Keyboard::Key::Escape) {
            mDebugOpen = false;
        }
    }
}

void WorldState::update(sf::Time dt) {
    if (mDebugOpen) return; // freeze movement while in debug UI

    const float speed = 220.f;
    sf::Vector2f move(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        move.y -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        move.y += speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        move.x -= speed;
        mPlayer.setTexture(playerReverse);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        move.x += speed;
        mPlayer.setTexture(playerTexture);
    }
        

    sf::Vector2f newPos = mPlayer.getPosition() + move * dt.asSeconds();

    // Collision logic
    if (mObstacleLocked) {
        sf::FloatRect nextBounds = mPlayer.getGlobalBounds();
        nextBounds.position = newPos;

        if (!nextBounds.findIntersection(mObstacle.getGlobalBounds()).has_value())
        {
            mPlayer.setPosition(newPos);
        }
    }
    else {
        mPlayer.setPosition(newPos);
    }

    // Camera follows player
    sf::FloatRect bounds = mPlayer.getGlobalBounds();
    sf::Vector2f playerCenter = 
    {
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f
    };
    mWorldView.setCenter(playerCenter);
}

void WorldState::render(sf::RenderTarget& target) {
    // Draw world with camera view
    target.setView(mWorldView);

    // Map first (background)
    target.draw(mMap);

    target.draw(mObstacle);
    target.draw(mPlayer);

    // Draw overlay/UI in screen space (not affected by camera)
    if (mDebugOpen) {
        target.setView(target.getDefaultView());
        target.draw(mOverlay);
        // add text/UI
    }
}