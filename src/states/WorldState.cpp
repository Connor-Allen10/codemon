#include "WorldState.hpp"
#include <cmath>

static float length(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

WorldState::WorldState(sf::RenderWindow& window)
: mWindow(window)
{
    // Player
    mPlayer.setSize({40.f, 40.f});
    mPlayer.setPosition({120.f, 200.f});
    mPlayer.setFillColor(sf::Color::Cyan);

    // Obstacle (door/bridge/etc.)
    mObstacle.setSize({80.f, 80.f});
    mObstacle.setPosition({500.f, 200.f});
    mObstacle.setFillColor(sf::Color::Red);

    // Debug overlay (simple for now)
    mOverlay.setSize(sf::Vector2f(mWindow.getSize()));
    mOverlay.setFillColor(sf::Color(0, 0, 0, 160));
}

bool WorldState::isNearObstacle() const {
    sf::Vector2f p = mPlayer.getPosition();
    sf::Vector2f o = mObstacle.getPosition();
    // crude proximity check
    return length((p + sf::Vector2f(20.f, 20.f)) - (o + sf::Vector2f(40.f, 40.f))) < 120.f;
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
                mObstacle.setFillColor(sf::Color::Green);
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        move.x -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        move.x += speed;

    sf::Vector2f newPos = mPlayer.getPosition() + move * dt.asSeconds();

    // Very simple collision: if obstacle is locked, keep player out of it
    if (mObstacleLocked) {
        sf::FloatRect nextBounds(newPos, mPlayer.getSize());
        if (!nextBounds.findIntersection(mObstacle.getGlobalBounds()).has_value()) {
            mPlayer.setPosition(newPos);
        }
    } else {
        mPlayer.setPosition(newPos);
    }
}

void WorldState::render(sf::RenderTarget& target) {
    target.draw(mObstacle);
    target.draw(mPlayer);

    // Draw overlay if debug open
    if (mDebugOpen) {
        target.draw(mOverlay);
        // (We’ll add text/UI next; keeping it simple so it compiles everywhere)
    }
}
