#include "states/WorldState.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

WorldState::WorldState(sf::RenderWindow& window)
: mWindow(window) {

    // Load map (try both common working-directory cases)
    bool loaded =
        mMap.loadFromCSV("assets/data/map01.csv", kTileSize) ||
        mMap.loadFromCSV("../assets/data/map01.csv", kTileSize);

    if (!loaded) {
        std::cout << "TileMap failed to load map01.csv; using whatever TileMap fallback exists.\n";
        // If your TileMap has a fallback method, call it here.
        // mMap.loadDefault();
    }

    setupView();
    tryLoadSprites();

    // Start positions (safe defaults)
    if (mPlayer) mPlayer->setPosition({2.f * kTileSize, 2.f * kTileSize});
    mPlayerFallback.setPosition({2.f * kTileSize, 2.f * kTileSize});

    if (mNpc) mNpc->setPosition({6.f * kTileSize, 4.f * kTileSize});
    mNpcFallback.setPosition({6.f * kTileSize, 4.f * kTileSize});

    updateViewCenterOnPlayer();
    clampViewToWorld();
}

void WorldState::setupView() {
    // Match the window size
    const auto ws = mWindow.getSize();
    mWorldView = sf::View(sf::FloatRect(
        sf::Vector2f{0.f, 0.f},
        sf::Vector2f{static_cast<float>(ws.x), static_cast<float>(ws.y)}
    ));
}

void WorldState::tryLoadSprites() {
    // Player texture
    bool playerOK =
        mPlayerTex.loadFromFile("assets/player.png") ||
        mPlayerTex.loadFromFile("../assets/player.png");

    // NPC texture (your file is npc.png)
    bool npcOK =
        mNpcTex.loadFromFile("assets/npc.png") ||
        mNpcTex.loadFromFile("../assets/npc.png");

    // Fallback rectangles (always valid)
    mPlayerFallback.setSize({static_cast<float>(kTileSize), static_cast<float>(kTileSize)});
    mPlayerFallback.setFillColor(sf::Color::Blue);

    mNpcFallback.setSize({static_cast<float>(kTileSize), static_cast<float>(kTileSize)});
    mNpcFallback.setFillColor(sf::Color::Red);

    if (playerOK) {
        mPlayer.emplace(mPlayerTex);
        mPlayer->setOrigin(sf::Vector2f{0.f, 0.f});
        scaleSpriteToOneTile(*mPlayer); // ✅ 1 tile player
    } else {
        std::cout << "WARNING: failed to load player.png; using fallback rectangle.\n";
    }

    if (npcOK) {
        mNpc.emplace(mNpcTex);
        mNpc->setOrigin(sf::Vector2f{0.f, 0.f});
        scaleSpriteToOneTile(*mNpc); // ✅ 1 tile NPC (your requested change)
    } else {
        std::cout << "WARNING: failed to load npc.png; using fallback rectangle.\n";
    }
}

void WorldState::scaleSpriteToOneTile(sf::Sprite& s) const {
    const auto b = s.getLocalBounds();            // SFML 3: b.position, b.size
    const float w = b.size.x;
    const float h = b.size.y;

    if (w <= 0.f || h <= 0.f) return;

    const float tile = static_cast<float>(kTileSize);
    s.setScale(sf::Vector2f{tile / w, tile / h});
}

sf::FloatRect WorldState::playerBoundsAt(const sf::Vector2f& pos) const {
    if (mPlayer) {
        auto b = mPlayer->getGlobalBounds();
        return sf::FloatRect(pos, b.size);
    }
    // fallback
    auto b = mPlayerFallback.getGlobalBounds();
    return sf::FloatRect(pos, b.size);
}

void WorldState::movePlayerWithCollision(const sf::Vector2f& delta) {
    // Axis-separated movement is usually cleaner for tile collision
    // X
    {
        sf::Vector2f curPos = mPlayer ? mPlayer->getPosition() : mPlayerFallback.getPosition();
        sf::Vector2f nextPos = {curPos.x + delta.x, curPos.y};

        sf::FloatRect nextBounds = playerBoundsAt(nextPos);

        // IMPORTANT: use the function your TileMap currently supports.
        // Based on your working version earlier: overlapsImpassable(bounds) == true means blocked.
        if (!mMap.overlapsImpassable(nextBounds)) {
            if (mPlayer) mPlayer->setPosition(nextPos);
            mPlayerFallback.setPosition(nextPos);
        }
    }

    // Y
    {
        sf::Vector2f curPos = mPlayer ? mPlayer->getPosition() : mPlayerFallback.getPosition();
        sf::Vector2f nextPos = {curPos.x, curPos.y + delta.y};

        sf::FloatRect nextBounds = playerBoundsAt(nextPos);

        if (!mMap.overlapsImpassable(nextBounds)) {
            if (mPlayer) mPlayer->setPosition(nextPos);
            mPlayerFallback.setPosition(nextPos);
        }
    }
}

void WorldState::updateViewCenterOnPlayer() {
    const sf::Vector2f p = mPlayer ? mPlayer->getPosition() : mPlayerFallback.getPosition();
    const sf::Vector2f center = {p.x + kTileSize * 0.5f, p.y + kTileSize * 0.5f};
    mWorldView.setCenter(center);
}

void WorldState::clampViewToWorld() {
    // If your TileMap exposes pixel/world size helpers, clamp to those.
    // If not, this keeps the view centered on player without hard clamping.
    // (Leaving this function here so you can clamp once you add map dimension getters.)
}

void WorldState::handleEvent(const sf::Event& e) {
    // SFML 3 event API
    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        const auto code = keyPressed->code;

        // If you want interaction keys here later (E to interact), add them here.
        if (code == sf::Keyboard::Key::Escape) {
            // placeholder: could open pause menu later
        }
    }
}

void WorldState::update(sf::Time dt) {
    sf::Vector2f dir{0.f, 0.f};

    // SFML 3 keyboard API uses sf::Keyboard::Key::<KeyName>
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        dir.y -= 1.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        dir.y += 1.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        dir.x -= 1.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        dir.x += 1.f;

    // normalize so diagonal isn't faster
    const float mag = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (mag > 0.f) dir /= mag;

    const sf::Vector2f delta = dir * (mMoveSpeed * dt.asSeconds());
    movePlayerWithCollision(delta);

    updateViewCenterOnPlayer();
    clampViewToWorld();
}

void WorldState::render(sf::RenderTarget& target) {
    target.setView(mWorldView);

    // TileMap is drawable in your current setup (you were drawing it successfully)
    target.draw(mMap);

    if (mNpc) target.draw(*mNpc);
    else      target.draw(mNpcFallback);

    if (mPlayer) target.draw(*mPlayer);
    else         target.draw(mPlayerFallback);
}