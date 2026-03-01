#include "WorldState.hpp"

#include <cmath>
#include <iostream>

static float vecLen(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

WorldState::WorldState(sf::RenderWindow& window)
: mWindow(window)
, mWorldView(window.getDefaultView())
, mMap(48)
, mDummyTex()
, mPlayerTex()
, mTrainerTex()
, mPlayer(mDummyTex)
, mTrainer(mDummyTex)
{
    // --- Build a 1x1 white dummy texture (SFML 3: Image has a size+color ctor)
    {
        sf::Image img({1u, 1u}, sf::Color::White);
        const bool ok = mDummyTex.loadFromImage(img);
        if (!ok) std::cerr << "Failed to create dummy texture.\n";
    }

    // --- Load map
    if (!mMap.loadFromCSV("assets/data/map01.csv", 48) &&
        !mMap.loadFromCSV("../assets/data/map01.csv", 48)) {
        std::cerr << "Failed to load map01.csv\n";
    }

    const float tile = static_cast<float>(mMap.getTileSize());

    // --- Load textures
    const bool pOk =
        mPlayerTex.loadFromFile("assets/player.png") ||
        mPlayerTex.loadFromFile("../assets/player.png");

    mTrainerTexLoaded =
        mTrainerTex.loadFromFile("assets/npc.png") ||
        mTrainerTex.loadFromFile("../assets/npc.png");

    if (pOk) mPlayer.setTexture(mPlayerTex, true);
    if (mTrainerTexLoaded) mTrainer.setTexture(mTrainerTex, true);

    // --- PLAYER: scale to exactly 1 tile
    {
        const sf::FloatRect b = mPlayer.getLocalBounds();
        const float w = b.size.x;
        const float h = b.size.y;
        if (w > 0.f && h > 0.f) {
            mPlayer.setScale({ tile / w, tile / h });
        }
        mPlayer.setOrigin({0.f, 0.f});
    }

    // --- TRAINER: 2x2 tiles if texture is present
    if (mTrainerTexLoaded) {
        const sf::FloatRect b = mTrainer.getLocalBounds();
        const float w = b.size.x;
        const float h = b.size.y;
        if (w > 0.f && h > 0.f) {
            mTrainer.setScale({ (2.f * tile) / w, (2.f * tile) / h });
        }
        mTrainer.setOrigin({0.f, 0.f});
    }

    // --- Fallback trainer rectangle (ALWAYS visible if texture fails)
    mTrainerFallback.setSize({ 2.f * tile, 2.f * tile });
    mTrainerFallback.setFillColor(sf::Color(200, 40, 40));
    mTrainerFallback.setOutlineThickness(2.f);
    mTrainerFallback.setOutlineColor(sf::Color::Black);

    // --- Place actors
    mPlayer.setPosition({ 5.f * tile, 5.f * tile });

    const sf::Vector2f trainerPos{ 12.f * tile, 6.f * tile };
    mTrainer.setPosition(trainerPos);
    mTrainerFallback.setPosition(trainerPos);

    // --- Overlay
    mOverlay.setSize(sf::Vector2f(mWindow.getSize()));
    mOverlay.setFillColor(sf::Color(0, 0, 0, 160));

    // camera start
    const sf::FloatRect pb = mPlayer.getGlobalBounds();
    mWorldView.setCenter({ pb.position.x + pb.size.x * 0.5f, pb.position.y + pb.size.y * 0.5f });
}

bool WorldState::isNearTrainer() const {
    const sf::Vector2f p = mPlayer.getPosition();
    const sf::Vector2f t = mTrainerTexLoaded ? mTrainer.getPosition() : mTrainerFallback.getPosition();

    // center-ish distance check
    return vecLen((p + sf::Vector2f(24.f, 24.f)) - (t + sf::Vector2f(48.f, 48.f))) < 120.f;
}

void WorldState::handleEvent(const sf::Event& e) {
    if (e.is<sf::Event::KeyPressed>()) {
        const auto* kp = e.getIf<sf::Event::KeyPressed>();
        if (!kp) return;

        if (kp->code == sf::Keyboard::Key::E) {
            if (mTrainerLocked && isNearTrainer()) {
                mDebugOpen = !mDebugOpen;
            }
        }

        if (kp->code == sf::Keyboard::Key::Escape) {
            mDebugOpen = false;
        }

        if (kp->code == sf::Keyboard::Key::Enter) {
            if (mDebugOpen) {
                mTrainerLocked = false;
                mDebugOpen = false;
            }
        }
    }
}

void WorldState::movePlayerWithCollision(sf::Vector2f delta) {
    // X
    if (delta.x != 0.f) {
        sf::Vector2f newPos = mPlayer.getPosition();
        newPos.x += delta.x;

        sf::FloatRect next = mPlayer.getGlobalBounds();
        next.position.x += delta.x;

        if (!mMap.overlapsImpassable(next)) {
            mPlayer.setPosition(newPos);
        }
    }

    // Y
    if (delta.y != 0.f) {
        sf::Vector2f newPos = mPlayer.getPosition();
        newPos.y += delta.y;

        sf::FloatRect next = mPlayer.getGlobalBounds();
        next.position.y += delta.y;

        if (!mMap.overlapsImpassable(next)) {
            mPlayer.setPosition(newPos);
        }
    }
}

void WorldState::update(sf::Time dt) {
    if (mDebugOpen) return;

    sf::Vector2f move(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))   move.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) move.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) move.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))move.x += 1.f;

    // normalize
    if (move.x != 0.f || move.y != 0.f) {
        const float len = vecLen(move);
        move /= len;
    }

    movePlayerWithCollision(move * (mSpeed * dt.asSeconds()));

    // camera follow
    const sf::FloatRect pb = mPlayer.getGlobalBounds();
    mWorldView.setCenter({ pb.position.x + pb.size.x * 0.5f, pb.position.y + pb.size.y * 0.5f });
}

void WorldState::render(sf::RenderTarget& target) {
    target.setView(mWorldView);

    target.draw(mMap);

    // draw trainer (sprite if loaded, otherwise fallback rectangle)
    if (mTrainerTexLoaded) target.draw(mTrainer);
    else                  target.draw(mTrainerFallback);

    target.draw(mPlayer);

    if (mDebugOpen) {
        target.setView(mWindow.getDefaultView());
        target.draw(mOverlay);
        target.setView(mWorldView);
    }
}