/**
 * @file WorldState.cpp
 * @brief Implementation of the WorldState class.
 */

#include "WorldState.hpp"
#include "BattleState.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#if __has_include(<TGUI/TGUI.hpp>) && __has_include(<TGUI/Backend/SFML-Graphics.hpp>)
#define CODEMON_HAS_TGUI 1
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#else
#define CODEMON_HAS_TGUI 0
#endif

namespace {
sf::Texture makeDummyTexture() {
    sf::Texture tex;
#if SFML_VERSION_MAJOR >= 3
    tex = sf::Texture(sf::Vector2u{1u, 1u});
#else
    tex.create(1u, 1u);
#endif
    return tex;
}
}

namespace {
constexpr unsigned kTileSize = 48;
constexpr int kFrameSize = 48;
constexpr float kAnimFrameTime = 0.14f;

sf::Vector2f normalizeOrZero(sf::Vector2f v) {
    const float mag = std::sqrt(v.x * v.x + v.y * v.y);
    if (mag <= 0.f) {
        return {0.f, 0.f};
    }
    return v / mag;
}

bool loadTextureFromAny(sf::Texture& tex, std::initializer_list<const char*> paths) {
    for (const char* path : paths) {
        // Skip non-existent candidates so SFML doesn't emit noisy
        // "Failed to load image" logs for every fallback path.
        std::error_code ec;
        if (!std::filesystem::exists(path, ec) || ec) {
            continue;
        }
        if (tex.loadFromFile(path)) {
            tex.setSmooth(false);
            return true;
        }
    }
    return false;
}
}

WorldState::WorldState(sf::RenderWindow& window)
: mWindow(window)
, mDummyTex(makeDummyTexture())
, mPlayer(mDummyTex)
, mTrainer(mDummyTex) {
    const bool mapLoaded =
        mMap.loadFromCSV("src/assets/data/map01.csv", kTileSize) ||
        mMap.loadFromCSV("assets/data/map01.csv", kTileSize) ||
        mMap.loadFromCSV("../src/assets/data/map01.csv", kTileSize) ||
        mMap.loadFromCSV("../assets/data/map01.csv", kTileSize);

    if (!mapLoaded) {
        std::cerr << "ERROR: Failed to load map file.\n";
    }

    const bool southLoaded = loadTextureFromAny(mPlayerTexSouth, {
        "src/assets/player_south.png", "assets/player_south.png",
        "../src/assets/player_south.png", "../assets/player_south.png"
    });
    const bool northLoaded = loadTextureFromAny(mPlayerTexNorth, {
        "src/assets/player_north.png", "assets/player_north.png",
        "../src/assets/player_north.png", "../assets/player_north.png"
    });
    const bool eastLoaded = loadTextureFromAny(mPlayerTexEast, {
        "src/assets/player_east.png", "assets/player_east.png",
        "../src/assets/player_east.png", "../assets/player_east.png"
    });
    const bool westLoaded = loadTextureFromAny(mPlayerTexWest, {
        "src/assets/player_west.png", "assets/player_west.png",
        "../src/assets/player_west.png", "../assets/player_west.png"
    });
    mPlayerTexLoaded = southLoaded && northLoaded && eastLoaded && westLoaded;

    if (!mPlayerTexLoaded) {
        std::cerr << "WARNING: Failed to load one or more directional player textures. Using fallback rectangle.\n";
    }

    mTrainerTexLoaded = loadTextureFromAny(mTrainerTex, {
        "src/assets/npc.png", "assets/npc.png",
        "../src/assets/npc.png", "../assets/npc.png"
    });
    if (!mTrainerTexLoaded) {
        std::cerr << "WARNING: Failed to load trainer texture. Using fallback rectangle.\n";
    }

    if (mPlayerTexLoaded) {
        updatePlayerVisual();
    }

    if (mTrainerTexLoaded) {
        mTrainer.setTexture(mTrainerTex, true);
        const auto tb = mTrainer.getLocalBounds();
#if SFML_VERSION_MAJOR >= 3
        if (tb.size.x > 0.f && tb.size.y > 0.f) {
            mTrainer.setScale({kTileSize / tb.size.x, kTileSize / tb.size.y});
        }
#else
        if (tb.width > 0.f && tb.height > 0.f) {
            mTrainer.setScale(kTileSize / tb.width, kTileSize / tb.height);
        }
#endif
    }

    // mTrainer.setPosition({300.f, 200.f});
    mTrainer.setColor(sf::Color(0,0,0,0));

    mPlayerFallback.setSize({static_cast<float>(kTileSize), static_cast<float>(kTileSize)});
    mPlayerFallback.setFillColor(sf::Color::Blue);

    mTrainerFallback.setSize({static_cast<float>(kTileSize), static_cast<float>(kTileSize)});
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

sf::Texture& WorldState::textureForFacing(Facing facing) {
    switch (facing) {
        case Facing::North: return mPlayerTexNorth;
        case Facing::East:  return mPlayerTexEast;
        case Facing::West:  return mPlayerTexWest;
        case Facing::South:
        default:            return mPlayerTexSouth;
    }
}

const sf::Texture& WorldState::textureForFacing(Facing facing) const {
    switch (facing) {
        case Facing::North: return mPlayerTexNorth;
        case Facing::East:  return mPlayerTexEast;
        case Facing::West:  return mPlayerTexWest;
        case Facing::South:
        default:            return mPlayerTexSouth;
    }
}

sf::IntRect WorldState::frameRect2x2(int frame) const {
    frame = std::clamp(frame, 0, 3);
    const int col = frame % 2;
    const int row = frame / 2;
#if SFML_VERSION_MAJOR >= 3
    return sf::IntRect({col * kFrameSize, row * kFrameSize}, {kFrameSize, kFrameSize});
#else
    return sf::IntRect(col * kFrameSize, row * kFrameSize, kFrameSize, kFrameSize);
#endif
}

sf::Vector2f WorldState::frameOffset(Facing facing, int frame) const {
    static const std::array<std::array<sf::Vector2f, 4>, 4> offsets{{
        // South: keep the old horizontal registration that matched the south sheet well.
        {{{-3.f, 2.f}, { 2.f, 2.f}, {-4.f, 2.f}, { 3.f, 2.f}}},
        // North
        {{{ 0.f, 0.f}, { 0.f, 0.f}, { 0.f, 0.f}, { 0.f, 0.f}}},
        // East
        {{{ 0.f, 0.f}, { 0.f, 0.f}, { 0.f, 0.f}, { 0.f, 0.f}}},
        // West
        {{{ 0.f, 0.f}, { 0.f, 0.f}, { 0.f, 0.f}, { 0.f, 0.f}}}
    }};

    return offsets[static_cast<std::size_t>(facing)][static_cast<std::size_t>(std::clamp(frame, 0, 3))];
}

void WorldState::updatePlayerVisual() {
    if (!mPlayerTexLoaded) {
        syncFallbackPositions();
        return;
    }

    mPlayer.setTexture(textureForFacing(mFacing), true);
    mPlayer.setTextureRect(frameRect2x2(mAnimIndex));
#if SFML_VERSION_MAJOR >= 3
    mPlayer.setScale({1.f, 1.f});
#else
    mPlayer.setScale(1.f, 1.f);
#endif
    mPlayer.setPosition(mPlayerTopLeft + frameOffset(mFacing, mAnimIndex));
    syncFallbackPositions();
}

sf::FloatRect WorldState::playerCollisionRectAt(sf::Vector2f topLeft) const {
    constexpr float width = 28.f;
    constexpr float height = 16.f;
    constexpr float xInset = (kTileSize - width) * 0.5f;
    constexpr float yOffset = kTileSize - height;
#if SFML_VERSION_MAJOR >= 3
    return sf::FloatRect({topLeft.x + xInset, topLeft.y + yOffset}, {width, height});
#else
    return sf::FloatRect(topLeft.x + xInset, topLeft.y + yOffset, width, height);
#endif
}

sf::Vector2f WorldState::getPlayerCenter() const {
    return {mPlayerTopLeft.x + kTileSize * 0.5f, mPlayerTopLeft.y + kTileSize * 0.5f};
}

void WorldState::syncFallbackPositions() {
    mPlayerFallback.setPosition(mPlayerTopLeft);
    mTrainerFallback.setPosition(mTrainer.getPosition());
}

void WorldState::updateCamera() {
    auto center = getPlayerCenter();
    center.x = std::round(center.x);
    center.y = std::round(center.y);
    mWorldView.setCenter(center);

    const auto mapWidth = static_cast<float>(mMap.getWidth() * mMap.getTileSize());
    const auto mapHeight = static_cast<float>(mMap.getHeight() * mMap.getTileSize());
    const auto viewHalfWidth = mWorldView.getSize().x * 0.5f;
    const auto viewHalfHeight = mWorldView.getSize().y * 0.5f;

    center = mWorldView.getCenter();

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

    center.x = std::round(center.x);
    center.y = std::round(center.y);
    mWorldView.setCenter(center);
}

bool WorldState::isNearTrainer() const {
    const auto playerPos = getPlayerCenter();
    const auto trainerPos = mTrainer.getPosition();
    const auto dx = playerPos.x - trainerPos.x;
    const auto dy = playerPos.y - trainerPos.y;
    const auto distSq = dx * dx + dy * dy;
    const float threshold = 96.f;
    return distSq < threshold * threshold;
}

void WorldState::movePlayerWithCollision(sf::Vector2f delta) {
    {
        const sf::Vector2f nextPos{mPlayerTopLeft.x + delta.x, mPlayerTopLeft.y};
        if (!mMap.overlapsImpassable(playerCollisionRectAt(nextPos))) {
            mPlayerTopLeft = nextPos;
        }
    }

    {
        const sf::Vector2f nextPos{mPlayerTopLeft.x, mPlayerTopLeft.y + delta.y};
        if (!mMap.overlapsImpassable(playerCollisionRectAt(nextPos))) {
            mPlayerTopLeft = nextPos;
        }
    }

    mPlayerTopLeft.x = std::round(mPlayerTopLeft.x);
    mPlayerTopLeft.y = std::round(mPlayerTopLeft.y);
    updatePlayerVisual();
}

sf::Vector2f WorldState::computeMovementInput(bool up, bool down, bool left, bool right) {
    sf::Vector2f movement{0.f, 0.f};

    if (up) {
        movement.y -= 1.f;
    }
    if (down) {
        movement.y += 1.f;
    }
    if (left) {
        movement.x -= 1.f;
    }
    if (right) {
        movement.x += 1.f;
    }

    return normalizeOrZero(movement);
}

void WorldState::applyMovement(sf::Vector2f move, sf::Time dt) {
    if (move.x == 0.f && move.y == 0.f) {
        updatePlayerVisual();
        updateCamera();
        return;
    }

    if (std::abs(move.x) > std::abs(move.y)) {
        mFacing = (move.x > 0.f) ? Facing::East : Facing::West;
    } else {
        mFacing = (move.y > 0.f) ? Facing::South : Facing::North;
    }

    mAnimTimer += dt.asSeconds();
    if (mAnimTimer >= kAnimFrameTime) {
        mAnimTimer -= kAnimFrameTime;
        static const int walkSequence[4] = {0, 1, 2, 3};
        mAnimIndex = walkSequence[(mAnimIndex + 1) % 4];
    }

    movePlayerWithCollision(move * (mSpeed * dt.asSeconds()));
    updateCamera();
}

void WorldState::handleEvent(const sf::Event& e) {
#if SFML_VERSION_MAJOR >= 3
    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::F1 || keyPressed->code == sf::Keyboard::Key::E) {
            helpMenuOpen = !helpMenuOpen;
        }
    }
    if (const auto* resized = e.getIf<sf::Event::Resized>()) {
        mWorldView.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
        mOverlay.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
        updateCamera();
    }
#else
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::F1 || e.key.code == sf::Keyboard::E) {
            helpMenuOpen = !helpMenuOpen;
        }
    }
    if (e.type == sf::Event::Resized) {
#if SFML_VERSION_MAJOR >= 3
        mWorldView.setSize({static_cast<float>(e.size.width), static_cast<float>(e.size.height)});
        mOverlay.setSize({static_cast<float>(e.size.width), static_cast<float>(e.size.height)});
#else
        mWorldView.setSize(static_cast<float>(e.size.width), static_cast<float>(e.size.height));
    mOverlay.setSize(sf::Vector2f(static_cast<float>(e.size.width), static_cast<float>(e.size.height)));
#endif
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

    const auto move = computeMovementInput(up, down, left, right);

    if (move.x != 0.f || move.y != 0.f) {
        applyMovement(move, dt);
    } else {
        mAnimIndex = 0;
        mAnimTimer = 0.f;
        updatePlayerVisual();
        updateCamera();
    }

    if (mEncounterCooldown > 0.f) {
        mEncounterCooldown -= dt.asSeconds();
    } else {
        checkEncounter();
        if (mEncounterCooldown <= 0.f) {
            mEncounterCooldown = kEncounterCheckInterval;
        }
    }
}

void WorldState::checkEncounter() {
    const auto playerCenter = getPlayerCenter();
    if (mMap.isEncounterAt(playerCenter)) {
        const float roll = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        if (roll < kEncounterChance) {
#ifndef NDEBUG
            std::cout << "[WorldState] Wild encounter! Transitioning to battle...\n";
#endif
            requestPush(std::make_unique<BattleState>(mWindow, mParty));
            mEncounterCooldown = 5.0f;
        }
    }
}

void WorldState::render(sf::RenderTarget& target) {
    target.setView(mWorldView);
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

    if (helpMenuOpen) {
        target.setView(target.getDefaultView());
    #if CODEMON_HAS_TGUI

        // CREATE HELP MENU
        sf::RenderWindow window(sf::VideoMode({800, 600}), "Help Menu");
        tgui::Gui gui{window};

        // Main background panel
        auto helpPanel = tgui::Panel::create({"70%", "70%"});
        helpPanel->setPosition({"15%", "15%"});
        helpPanel->getRenderer()->setBackgroundColor(tgui::Color(180, 180, 180));
        helpPanel->getRenderer()->setBorderColor(tgui::Color(180, 180, 180));
        helpPanel->getRenderer()->setBorders(2);
        gui.add(helpPanel);

        // Title
        auto title = tgui::Label::create("Help Menu");
        title->setTextSize(28);
        title->setPosition({"(&.width - width) / 2", "4%"});
        helpPanel->add(title);

        // ===== CONTROLS SECTION =====
        auto controlsTitle = tgui::Label::create("Controls");
        controlsTitle->setTextSize(22);
        controlsTitle->setPosition({"5%", "15%"});
        helpPanel->add(controlsTitle);

        auto controlsLine = tgui::SeparatorLine::create({"90%", 2});
        controlsLine->setPosition({"5%", "20%"});
        helpPanel->add(controlsLine);

        auto controlsText = tgui::Label::create(
            "W / A / S / D  - Move character\n"
            "E  - Open GUI\n"
            "ESC/Enter  - Close GUI\n"
        );
        controlsText->setTextSize(18);
        controlsText->setPosition({"5%", "23%"});
        helpPanel->add(controlsText);

        // Game Description
        auto descTitle = tgui::Label::create("Game Description");
        descTitle->setTextSize(22);
        descTitle->setPosition({"5%", "50%"});
        helpPanel->add(descTitle);

        auto descLine = tgui::SeparatorLine::create({"90%", 2});
        descLine->setPosition({"5%", "55%"});
        helpPanel->add(descLine);

        auto descText = tgui::Label::create(
            "Try moving around the world until you encounter\n"
            "A Codemon, and use your debugging skills to defeat it!\n"
        );
        descText->setTextSize(18);
        descText->setPosition({"5%", "58%"});
        helpPanel->add(descText);

        while (window.isOpen())
        {
            while (const std::optional event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>()) {
                    helpMenuOpen = !helpMenuOpen;
                    window.close();
                }
                
                gui.handleEvent(*event);

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
                    helpMenuOpen = !helpMenuOpen;
                    window.close();
                }
                    
            }

            window.clear();
            gui.draw();
            window.display();
        }
    #else
        target.draw(mOverlay);
    #endif
    }
}
