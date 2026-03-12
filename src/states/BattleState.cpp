/**
 * @file BattleState.cpp
 * @brief Implementation of the BattleState class.
 */

#include "BattleState.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#if __has_include(<TGUI/TGUI.hpp>) && __has_include(<TGUI/Backend/SFML-Graphics.hpp>)
#define CODEMON_HAS_TGUI 1
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#else
#define CODEMON_HAS_TGUI 0
#endif

Debug::ChallengeLoader BattleState::sChallengeLoader("challenges.txt");

namespace {
struct MonsterAsset {
    const char* fileName;
};

const std::vector<MonsterAsset>& monsterAssets() {
    static const std::vector<MonsterAsset> assets = {
        {"forest_mon.png"},
        {"rocky_mon.png"},
        {"Water_mon.png"},
        {"fire_bird_mon.png"},
        {"zombie_mon.png"},
        {"candy_mon.png"}
    };
    return assets;
}

bool loadTextureFromAny(sf::Texture& texture, std::initializer_list<const char*> paths) {
    for (const char* path : paths) {
        if (texture.loadFromFile(path)) {
            texture.setSmooth(true);
            return true;
        }
    }
    return false;
}

bool loadMonsterTexture(sf::Texture& texture, const std::string& fileName) {
    const std::string srcPath = std::string("src/assets/") + fileName;
    const std::string assetsPath = std::string("assets/") + fileName;
    const std::string parentSrcPath = std::string("../src/assets/") + fileName;
    const std::string parentAssetsPath = std::string("../assets/") + fileName;

    return loadTextureFromAny(texture, {
        srcPath.c_str(),
        assetsPath.c_str(),
        parentSrcPath.c_str(),
        parentAssetsPath.c_str()
    });
}

std::string pickRandomWildMon() {
    const auto& assets = monsterAssets();
    if (assets.empty()) {
        return "forest_mon.png";
    }
    const std::size_t index = static_cast<std::size_t>(std::rand()) % assets.size();
    return assets[index].fileName;
}

void fitSprite(sf::Sprite& sprite, float maxWidth, float maxHeight) {
#if SFML_VERSION_MAJOR >= 3
    const auto bounds = sprite.getLocalBounds();
    if (bounds.size.x <= 0.f || bounds.size.y <= 0.f) {
        return;
    }
    const float scale = std::min(maxWidth / bounds.size.x, maxHeight / bounds.size.y);
    sprite.setScale({scale, scale});
#else
    const auto bounds = sprite.getLocalBounds();
    if (bounds.width <= 0.f || bounds.height <= 0.f) {
        return;
    }
    const float scale = std::min(maxWidth / bounds.width, maxHeight / bounds.height);
    sprite.setScale(scale, scale);
#endif
}

void applySubmission(Debug::Engine& engine,
                     const std::string& submission,
                     std::string& message,
                     bool& solved,
                     bool& failed) {
    const ValidationResult result = engine.submit(submission);
    message = result.feedback;
    solved = result.success;
    failed = !result.success;
}

#if CODEMON_HAS_TGUI
std::optional<std::string> runDebugEditorPopup(const std::string& prompt,
                                               const std::string& keywordHint) {
#if SFML_VERSION_MAJOR >= 3
    sf::RenderWindow popup(sf::VideoMode({960, 640}), "CodeMon Debug Editor", sf::Style::Titlebar | sf::Style::Close);
#else
    sf::RenderWindow popup(sf::VideoMode(960, 640), "CodeMon Debug Editor", sf::Style::Titlebar | sf::Style::Close);
#endif

    tgui::Gui gui{popup};

    auto panel = tgui::Panel::create({"92%", "92%"});
    panel->setPosition({"4%", "4%"});
    panel->getRenderer()->setBackgroundColor(tgui::Color(45, 45, 55));
    gui.add(panel);

    auto title = tgui::Label::create("BATTLE DEBUG CHALLENGE");
    title->setPosition({"3%", "3%"});
    title->setTextSize(26);
    title->getRenderer()->setTextColor(tgui::Color::White);
    panel->add(title);

    auto promptLabel = tgui::Label::create("Something is wrong with this code!");
    promptLabel->setPosition({"3%", "13%"});
    promptLabel->setTextSize(18);
    promptLabel->setAutoSize(false);
    promptLabel->setSize({"94%", "10%"});
    promptLabel->getRenderer()->setTextColor(tgui::Color(220, 220, 220));
    panel->add(promptLabel);

    auto instructionLabel = tgui::Label::create("Type the corrected code below:");
    instructionLabel->setPosition({"3%", "24%"});
    instructionLabel->setTextSize(14);
    instructionLabel->getRenderer()->setTextColor(tgui::Color(180, 180, 180));
    panel->add(instructionLabel);

    auto editor = tgui::TextArea::create();
    editor->setPosition({"3%", "29%"});
    editor->setSize({"94%", "49%"});
    editor->setText(prompt);
    editor->setTextSize(22);
    editor->getRenderer()->setBackgroundColor(tgui::Color(55, 55, 65));
    editor->getRenderer()->setTextColor(tgui::Color::White);
    editor->getRenderer()->setCaretColor(tgui::Color::White);
    editor->getRenderer()->setSelectedTextBackgroundColor(tgui::Color(80, 80, 120));
    editor->getRenderer()->setBorderColor(tgui::Color(70, 70, 80));
    panel->add(editor);

    auto submitButton = tgui::Button::create("Submit");
    submitButton->setPosition({"67%", "85%"});
    submitButton->setSize({"14%", "10%"});
    submitButton->getRenderer()->setBackgroundColor(tgui::Color(60, 120, 60));
    submitButton->getRenderer()->setTextColor(tgui::Color::White);
    panel->add(submitButton);

    auto cancelButton = tgui::Button::create("Cancel");
    cancelButton->setPosition({"82%", "85%"});
    cancelButton->setSize({"14%", "10%"});
    cancelButton->getRenderer()->setBackgroundColor(tgui::Color(120, 60, 60));
    cancelButton->getRenderer()->setTextColor(tgui::Color::White);
    panel->add(cancelButton);

    const std::string hintText = keywordHint.empty()
        ? "No required keyword for this challenge."
        : ("Required keyword: " + keywordHint);
    auto hint = tgui::Label::create(hintText);
    hint->setPosition({"25%", "85%"});
    hint->setTextSize(14);
    hint->getRenderer()->setTextColor(tgui::Color(200, 200, 150));
    panel->add(hint);

    bool submitted = false;
    std::string submission;

    submitButton->onPress([&]() {
        submission = editor->getText().toStdString();
        submitted = true;
        popup.close();
    });

    cancelButton->onPress([&]() {
        popup.close();
    });

    while (popup.isOpen()) {
#if SFML_VERSION_MAJOR >= 3
        while (const std::optional event = popup.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                popup.close();
            }
            gui.handleEvent(*event);
        }
#else
        sf::Event event;
        while (popup.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                popup.close();
            }
            gui.handleEvent(event);
        }
#endif

        popup.clear(sf::Color(35, 35, 45));
        gui.draw();
        popup.display();
    }

    if (submitted) {
        return submission;
    }
    return std::nullopt;
}
#endif
}

BattleState::BattleState(sf::RenderWindow& window, std::string preferredPlayerMonFile)
: mWindow(window)
, mEnemySprite(mEnemyTexture)
, mPlayerSprite(mPlayerTexture)
, mPreferredPlayerMonFile(preferredPlayerMonFile)
, mWildMonFile(pickRandomWildMon()) {
    updateViewLayout(mWindow.getSize());

    mBackground.setFillColor(sf::Color(64, 0, 128));
    mEnemyPlatform.setFillColor(sf::Color(90, 70, 120, 180));
    mPlayerPlatform.setFillColor(sf::Color(70, 90, 130, 180));
    mEnemyPlatform.setSize({260.f, 44.f});
    mPlayerPlatform.setSize({300.f, 50.f});

    mEnemyTextureLoaded = loadMonsterTexture(mEnemyTexture, mWildMonFile);
    if (mEnemyTextureLoaded) {
        mEnemySprite.setTexture(mEnemyTexture, true);
        fitSprite(mEnemySprite, 250.f, 250.f);
    } else {
        std::cerr << "WARNING: Failed to load wild battle sprite: " << mWildMonFile << "\n";
    }

    const std::vector<std::string> fontCandidates = {
        "src/assets/fonts/arial.ttf",
        "assets/fonts/arial.ttf",
        "../src/assets/fonts/arial.ttf",
        "../assets/fonts/arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/Library/Fonts/Arial.ttf",
        "C:/Windows/Fonts/arial.ttf"
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
        std::cerr << "WARNING: Failed to load battle font. Text will not display.\n";
    }

    auto maybeChallenge = sChallengeLoader.getRandomChallenge();
    if (maybeChallenge) {
        mCurrentKeywordHint = maybeChallenge->keywordHint;
        mDebugEngine.startChallenge(*maybeChallenge);
    } else {
        mCurrentKeywordHint = "return";
        mDebugEngine.startChallenge(Debug::Challenge{
            "Fix bug: change 'retun 0;' to valid C++",
            "return 0;",
            "return"
        });
    }

#if CODEMON_HAS_TGUI
    mBattleMessage = "Battle started. Press E or F1 to open Debug Editor. ESC = exit.";
#else
    mBattleMessage = "Battle started. Press Enter to submit fix. Backspace = wrong attempt. ESC = exit.";
#endif

    beginPlayerSelection();
}

void BattleState::beginPlayerSelection() {
    mSelectionMonFiles.clear();
    mSelectionTextures.clear();
    mSelectionSprites.clear();
    mSelectionLoaded.clear();

    const auto& assets = monsterAssets();
    mSelectionMonFiles.reserve(assets.size());
    mSelectionTextures.reserve(assets.size());
    mSelectionSprites.reserve(assets.size());
    mSelectionLoaded.reserve(assets.size());

    std::size_t preferredIndex = 0;
    bool foundPreferred = false;

    for (const auto& asset : assets) {
        mSelectionMonFiles.emplace_back(asset.fileName);
        mSelectionTextures.emplace_back();
        const bool loaded = loadMonsterTexture(mSelectionTextures.back(), asset.fileName);
        mSelectionLoaded.push_back(loaded);
        if (loaded) {
            mSelectionSprites.emplace_back(mSelectionTextures.back());
            fitSprite(mSelectionSprites.back(), 108.f, 108.f);
        } else {
            mSelectionTextures.back() = mEnemyTexture;
            mSelectionSprites.emplace_back(mSelectionTextures.back());
            std::cerr << "WARNING: Failed to load selectable battle sprite: " << asset.fileName << "\n";
        }

        if (!mPreferredPlayerMonFile.empty() && mPreferredPlayerMonFile == asset.fileName) {
            preferredIndex = mSelectionMonFiles.size() - 1u;
            foundPreferred = true;
        }
    }

    if (foundPreferred) {
        mSelectionIndex = preferredIndex;
    } else {
        mSelectionIndex = 0;
    }
}

void BattleState::refreshPlayerSprite() {
    mPlayerTextureLoaded = false;
    if (mPlayerMonFile.empty()) {
        return;
    }

    mPlayerTextureLoaded = loadMonsterTexture(mPlayerTexture, mPlayerMonFile);
    if (mPlayerTextureLoaded) {
        mPlayerSprite.setTexture(mPlayerTexture, true);
        fitSprite(mPlayerSprite, 250.f, 250.f);
    } else {
        std::cerr << "WARNING: Failed to load selected player battle sprite: " << mPlayerMonFile << "\n";
    }
}

void BattleState::confirmPlayerSelection() {
    if (mSelectionMonFiles.empty()) {
        mPlayerMonFile = "forest_mon.png";
    } else {
        mPlayerMonFile = mSelectionMonFiles[mSelectionIndex];
    }
    refreshPlayerSprite();
    mPhase = Phase::DebugBattle;
}

void BattleState::moveSelection(int delta) {
    if (mSelectionMonFiles.empty()) {
        return;
    }

    const int count = static_cast<int>(mSelectionMonFiles.size());
    int next = static_cast<int>(mSelectionIndex) + delta;
    while (next < 0) {
        next += count;
    }
    next %= count;
    mSelectionIndex = static_cast<std::size_t>(next);
}

void BattleState::updateViewLayout(sf::Vector2u size) {
    mBattleView = sf::View(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(size.x), static_cast<float>(size.y)}
    ));
    mBackground.setPosition({0.f, 0.f});
    mBackground.setSize({static_cast<float>(size.x), static_cast<float>(size.y)});
}

void BattleState::handleEvent(const sf::Event& e) {
#if SFML_VERSION_MAJOR >= 3
    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        const auto code = keyPressed->code;
        if (code == sf::Keyboard::Key::Escape) {
            mExitRequested = true;
            requestPop();
            return;
        }

        if (mPhase == Phase::EncounterIntro) {
            return;
        }

        if (mPhase == Phase::PlayerSelect) {
            switch (code) {
                case sf::Keyboard::Key::Left:
                case sf::Keyboard::Key::A:
                    moveSelection(-1);
                    return;
                case sf::Keyboard::Key::Right:
                case sf::Keyboard::Key::D:
                    moveSelection(1);
                    return;
                case sf::Keyboard::Key::Num1: mSelectionIndex = 0; return;
                case sf::Keyboard::Key::Num2: if (mSelectionMonFiles.size() > 1) mSelectionIndex = 1; return;
                case sf::Keyboard::Key::Num3: if (mSelectionMonFiles.size() > 2) mSelectionIndex = 2; return;
                case sf::Keyboard::Key::Num4: if (mSelectionMonFiles.size() > 3) mSelectionIndex = 3; return;
                case sf::Keyboard::Key::Num5: if (mSelectionMonFiles.size() > 4) mSelectionIndex = 4; return;
                case sf::Keyboard::Key::Num6: if (mSelectionMonFiles.size() > 5) mSelectionIndex = 5; return;
                case sf::Keyboard::Key::Enter:
                case sf::Keyboard::Key::Space:
                    confirmPlayerSelection();
                    return;
                default:
                    return;
            }
        }

#if CODEMON_HAS_TGUI
        if ((code == sf::Keyboard::Key::E || code == sf::Keyboard::Key::F1) &&
            mDebugEngine.hasActiveChallenge()) {
            const auto submission = runDebugEditorPopup(mDebugEngine.currentPrompt(), mCurrentKeywordHint);
            if (submission.has_value()) {
                applySubmission(mDebugEngine, submission.value(), mBattleMessage, mChallengeSolved, mSubmissionFailed);
            }
        }
#else
        if (code == sf::Keyboard::Key::Enter && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "return 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
        if (code == sf::Keyboard::Key::Backspace && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "retun 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
#endif
    }
    if (const auto* resized = e.getIf<sf::Event::Resized>()) {
        updateViewLayout(resized->size);
    }
#else
    if (e.type == sf::Event::KeyPressed) {
        const auto code = e.key.code;
        if (code == sf::Keyboard::Escape) {
            mExitRequested = true;
            requestPop();
            return;
        }

        if (mPhase == Phase::EncounterIntro) {
            return;
        }

        if (mPhase == Phase::PlayerSelect) {
            switch (code) {
                case sf::Keyboard::Left:
                case sf::Keyboard::A:
                    moveSelection(-1);
                    return;
                case sf::Keyboard::Right:
                case sf::Keyboard::D:
                    moveSelection(1);
                    return;
                case sf::Keyboard::Num1: mSelectionIndex = 0; return;
                case sf::Keyboard::Num2: if (mSelectionMonFiles.size() > 1) mSelectionIndex = 1; return;
                case sf::Keyboard::Num3: if (mSelectionMonFiles.size() > 2) mSelectionIndex = 2; return;
                case sf::Keyboard::Num4: if (mSelectionMonFiles.size() > 3) mSelectionIndex = 3; return;
                case sf::Keyboard::Num5: if (mSelectionMonFiles.size() > 4) mSelectionIndex = 4; return;
                case sf::Keyboard::Num6: if (mSelectionMonFiles.size() > 5) mSelectionIndex = 5; return;
                case sf::Keyboard::Return:
                case sf::Keyboard::Space:
                    confirmPlayerSelection();
                    return;
                default:
                    return;
            }
        }

#if CODEMON_HAS_TGUI
        if ((code == sf::Keyboard::E || code == sf::Keyboard::F1) &&
            mDebugEngine.hasActiveChallenge()) {
            const auto submission = runDebugEditorPopup(mDebugEngine.currentPrompt(), mCurrentKeywordHint);
            if (submission.has_value()) {
                applySubmission(mDebugEngine, submission.value(), mBattleMessage, mChallengeSolved, mSubmissionFailed);
            }
        }
#else
        if (code == sf::Keyboard::Return && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "return 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
        if (code == sf::Keyboard::BackSpace && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "retun 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
#endif
    }
    if (e.type == sf::Event::Resized) {
        updateViewLayout(sf::Vector2u(static_cast<unsigned>(e.size.width), static_cast<unsigned>(e.size.height)));
    }
#endif
}

void BattleState::update(sf::Time dt) {
    mTimer += dt.asSeconds();
    if (mPhase == Phase::EncounterIntro) {
        mEncounterIntroTimer += dt.asSeconds();
        if (mEncounterIntroTimer >= kEncounterIntroDuration) {
            mPhase = Phase::PlayerSelect;
        }
    }
}

void BattleState::render(sf::RenderTarget& target) {
    target.setView(mBattleView);

    const float windowWidth = static_cast<float>(mWindow.getSize().x);
    const float windowHeight = static_cast<float>(mWindow.getSize().y);

    if (mChallengeSolved) {
        mBackground.setFillColor(sf::Color(20, 120, 40));
    } else if (mSubmissionFailed) {
        mBackground.setFillColor(sf::Color(150, 0, 0));
    } else {
        mBackground.setFillColor(sf::Color(64, 0, 128));
    }

    target.draw(mBackground);

    if (mPhase == Phase::EncounterIntro) {
        if (mEnemyTextureLoaded) {
#if SFML_VERSION_MAJOR >= 3
            const auto bounds = mEnemySprite.getGlobalBounds();
            mEnemySprite.setPosition({windowWidth * 0.5f - bounds.size.x * 0.5f,
                                      windowHeight * 0.56f - bounds.size.y * 0.5f});
#else
            const auto bounds = mEnemySprite.getGlobalBounds();
            mEnemySprite.setPosition(windowWidth * 0.5f - bounds.width * 0.5f,
                                     windowHeight * 0.56f - bounds.height * 0.5f);
#endif
            target.draw(mEnemySprite);
        }

        if (mFontLoaded) {
#if SFML_VERSION_MAJOR >= 3
            sf::Text title(mFont, "A wild Codemon appeared!", 42);
            sf::Text hint(mFont, "Get ready...", 24);
#else
            sf::Text title("A wild Codemon appeared!", mFont, 42);
            sf::Text hint("Get ready...", mFont, 24);
#endif
            title.setFillColor(sf::Color::White);
            hint.setFillColor(sf::Color(230, 230, 230));
#if SFML_VERSION_MAJOR >= 3
            const auto titleBounds = title.getLocalBounds();
            title.setOrigin({titleBounds.position.x + titleBounds.size.x * 0.5f,
                             titleBounds.position.y + titleBounds.size.y * 0.5f});
            const auto hintBounds = hint.getLocalBounds();
            hint.setOrigin({hintBounds.position.x + hintBounds.size.x * 0.5f,
                            hintBounds.position.y + hintBounds.size.y * 0.5f});
#else
            const auto titleBounds = title.getLocalBounds();
            title.setOrigin(titleBounds.left + titleBounds.width * 0.5f,
                            titleBounds.top + titleBounds.height * 0.5f);
            const auto hintBounds = hint.getLocalBounds();
            hint.setOrigin(hintBounds.left + hintBounds.width * 0.5f,
                           hintBounds.top + hintBounds.height * 0.5f);
#endif
            title.setPosition({windowWidth * 0.5f, windowHeight * 0.18f});
            hint.setPosition({windowWidth * 0.5f, windowHeight * 0.82f});
            target.draw(title);
            target.draw(hint);
        }
        return;
    }

    mEnemyPlatform.setPosition({windowWidth * 0.60f, windowHeight * 0.30f});
    //target.draw(mEnemyPlatform);

    if (mEnemyTextureLoaded) {
#if SFML_VERSION_MAJOR >= 3
        const auto bounds = mEnemySprite.getGlobalBounds();
        mEnemySprite.setPosition({windowWidth * 0.74f - bounds.size.x * 0.5f,
                                  windowHeight * 0.33f - bounds.size.y * 0.75f});
#else
        const auto bounds = mEnemySprite.getGlobalBounds();
        mEnemySprite.setPosition(windowWidth * 0.74f - bounds.width * 0.5f,
                                 windowHeight * 0.33f - bounds.height * 0.75f);
#endif
        target.draw(mEnemySprite);
    }

    if (mPhase == Phase::PlayerSelect) {
        if (mFontLoaded) {
#if SFML_VERSION_MAJOR >= 3
            sf::Text title(mFont, "Choose your Codemon", 34);
            sf::Text hint(mFont, "Left/Right or 1-6 to choose, Enter to confirm", 22);
#else
            sf::Text title("Choose your Codemon", mFont, 34);
            sf::Text hint("Left/Right or 1-6 to choose, Enter to confirm", mFont, 22);
#endif
            title.setFillColor(sf::Color::White);
            hint.setFillColor(sf::Color(230, 230, 230));
            title.setPosition({40.f, 34.f});
            hint.setPosition({40.f, 76.f});
            target.draw(title);
            target.draw(hint);
        }

        const float startX = 50.f;
        const float gap = 120.f;
        const float rowY = windowHeight * 0.70f;
        for (std::size_t i = 0; i < mSelectionSprites.size(); ++i) {
            sf::RectangleShape slotBg({96.f, 96.f});
            slotBg.setFillColor(i == mSelectionIndex ? sf::Color(255, 255, 255, 60)
                                                     : sf::Color(0, 0, 0, 55));
            slotBg.setOutlineThickness(3.f);
            slotBg.setOutlineColor(i == mSelectionIndex ? sf::Color(255, 235, 120)
                                                        : sf::Color(160, 160, 190));
            slotBg.setPosition({startX + gap * static_cast<float>(i), rowY});
            target.draw(slotBg);

            if (i < mSelectionLoaded.size() && mSelectionLoaded[i]) {
                auto& sprite = mSelectionSprites[i];
#if SFML_VERSION_MAJOR >= 3
                const auto bounds = sprite.getGlobalBounds();
                sprite.setPosition({slotBg.getPosition().x + 48.f - bounds.size.x * 0.5f,
                                    slotBg.getPosition().y + 50.f - bounds.size.y * 0.5f});
#else
                const auto bounds = sprite.getGlobalBounds();
                sprite.setPosition(slotBg.getPosition().x + 48.f - bounds.width * 0.5f,
                                   slotBg.getPosition().y + 50.f - bounds.height * 0.5f);
#endif
                target.draw(sprite);
            }

            if (mFontLoaded) {
#if SFML_VERSION_MAJOR >= 3
                sf::Text num(mFont, std::to_string(i + 1), 20);
#else
                sf::Text num(std::to_string(i + 1), mFont, 20);
#endif
                num.setFillColor(sf::Color::White);
                num.setPosition({slotBg.getPosition().x + 6.f, slotBg.getPosition().y + 4.f});
                target.draw(num);
            }
        }
        return;
    }

    mPlayerPlatform.setPosition({windowWidth * 0.10f, windowHeight * 0.70f});
    //target.draw(mPlayerPlatform);

    if (mPlayerTextureLoaded) {
#if SFML_VERSION_MAJOR >= 3
        const auto bounds = mPlayerSprite.getGlobalBounds();
        mPlayerSprite.setPosition({windowWidth * 0.24f - bounds.size.x * 0.5f,
                                   windowHeight * 0.73f - bounds.size.y * 0.80f});
#else
        const auto bounds = mPlayerSprite.getGlobalBounds();
        mPlayerSprite.setPosition(windowWidth * 0.24f - bounds.width * 0.5f,
                                  windowHeight * 0.73f - bounds.height * 0.80f);
#endif
        target.draw(mPlayerSprite);
    }

    if (mFontLoaded) {
#if SFML_VERSION_MAJOR >= 3
        sf::Text title(mFont, "BATTLE DEBUG", 40);
        sf::Text subtitle(mFont, mBattleMessage, 22);
        sf::Text prompt(mFont,
                        mDebugEngine.hasActiveChallenge()
                            ? "Can you find what's wrong with the following code?"
                            : "Challenge solved. Press ESC to return.",
                        20);
        sf::Text controls(mFont,
#if CODEMON_HAS_TGUI
                          "Controls: E/F1 open editor, ESC exits battle",
#else
                          "Controls: Enter=correct test, Backspace=wrong test, ESC exits",
#endif
                          18);
#else
        sf::Text title("BATTLE DEBUG", mFont, 40);
        sf::Text subtitle(mBattleMessage, mFont, 22);
        sf::Text prompt(mDebugEngine.hasActiveChallenge()
                            ? "Can you find what's wrong with the following code?"
                            : "Challenge solved. Press ESC to return.",
                        mFont,
                        20);
        sf::Text controls(
#if CODEMON_HAS_TGUI
            "Controls: E/F1 open editor, ESC exits battle",
#else
            "Controls: Enter=correct test, Backspace=wrong test, ESC exits",
#endif
            mFont,
            18);
#endif
        subtitle.setFillColor(sf::Color(230, 230, 230));
        prompt.setFillColor(sf::Color(220, 220, 140));
        controls.setFillColor(sf::Color(200, 200, 200));
#if SFML_VERSION_MAJOR >= 3
        const auto bounds = title.getLocalBounds();
        title.setOrigin({bounds.position.x + bounds.size.x * 0.5f,
                         bounds.position.y + bounds.size.y * 0.5f});
#else
        const auto bounds = title.getLocalBounds();
        title.setOrigin(bounds.left + bounds.width * 0.5f,
                        bounds.top + bounds.height * 0.5f);
#endif
        title.setPosition({windowWidth * 0.5f, windowHeight * 0.08f});
        subtitle.setPosition({40.f, windowHeight * 0.82f});
        prompt.setPosition({40.f, windowHeight * 0.88f});
        controls.setPosition({40.f, windowHeight * 0.93f});

        const float pulse = 0.5f + 0.5f * std::sin(mTimer * 3.f);
        const auto alpha = static_cast<std::uint8_t>(128 + 127 * pulse);
        title.setFillColor(sf::Color(255, 255, 255, alpha));

        target.draw(title);
        target.draw(subtitle);
        target.draw(prompt);
        target.draw(controls);
    }
}
