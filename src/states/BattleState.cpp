/**
 * @file BattleState.cpp
 * @brief Implementation of the BattleState class.
 */

#include "BattleState.hpp"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
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
sf::Texture makeDummyTexture() {
    sf::Texture tex;
#if SFML_VERSION_MAJOR >= 3
    tex = sf::Texture(sf::Vector2u{1u, 1u});
#else
    tex.create(1u, 1u);
#endif
    return tex;
}

bool loadTextureFromAny(sf::Texture& tex, std::initializer_list<const char*> paths) {
    for (const char* path : paths) {
        if (tex.loadFromFile(path)) {
            tex.setSmooth(false);
            return true;
        }
    }
    return false;
}

std::string chooseRandomMonsterAsset() {
    static const std::vector<std::string> monsters = {
        "forest_mon.png",
        "fire_bird_mon.png",
        "rocky_mon.png",
        "Water_mon.png",
        "candy_mon.png",
        "zombie_mon.png"
    };

    if (monsters.empty()) {
        return {};
    }

    const std::size_t index = static_cast<std::size_t>(std::rand()) % monsters.size();
    return monsters[index];
}

#if CODEMON_HAS_TGUI
std::optional<std::string> runDebugEditorPopup(const std::string& prompt,
                                               const std::string& keywordHint) {
#if SFML_VERSION_MAJOR >= 3
    sf::RenderWindow popup(sf::VideoMode({1120, 820}), "CodeMon Debug Editor", sf::Style::Titlebar | sf::Style::Close);
#else
    sf::RenderWindow popup(sf::VideoMode(1120, 820), "CodeMon Debug Editor", sf::Style::Titlebar | sf::Style::Close);
#endif

    tgui::Gui gui{popup};
    // Allow Tab to insert indentation inside TextArea instead of changing widget focus.
    gui.setTabKeyUsageEnabled(false);

    // Load monospace font for code-editor feel.
    // Prefer bundled JetBrains Mono, fall back to Courier New (cross-platform).
    tgui::Font monoFont;
    bool monoLoaded = false;
    const std::vector<std::string> monoFontCandidates = {
        "assets/fonts/JetBrainsMono-Regular.ttf",
        "../assets/fonts/JetBrainsMono-Regular.ttf",
        "src/assets/fonts/JetBrainsMono-Regular.ttf",
        "/System/Library/Fonts/Supplemental/Courier New.ttf",
        "C:/Windows/Fonts/cour.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
    };
    for (const auto& path : monoFontCandidates) {
        std::error_code ec;
        if (!std::filesystem::exists(path, ec) || ec) continue;
        monoFont = tgui::Font(path);
        monoLoaded = true;
        break;
    }

    auto panel = tgui::Panel::create({"92%", "92%"});
    panel->setPosition({"4%", "4%"});
    panel->getRenderer()->setBackgroundColor(tgui::Color(45, 45, 55));
    gui.add(panel);

    auto title = tgui::Label::create("BATTLE DEBUG CHALLENGE");
    title->setPosition({"3%", "3%"});
    title->setTextSize(26);
    title->getRenderer()->setTextColor(tgui::Color::White);
    panel->add(title);

    auto promptLabel = tgui::Label::create("Broken code:");
    promptLabel->setPosition({"3%", "12%"});
    promptLabel->setTextSize(16);
    promptLabel->getRenderer()->setTextColor(tgui::Color(210, 210, 210));
    panel->add(promptLabel);

    auto promptView = tgui::TextArea::create();
    promptView->setPosition({"3%", "16%"});
    promptView->setSize({"94%", "31%"});
    promptView->setText(prompt);
    promptView->setReadOnly(true);
    promptView->setTextSize(18);
    if (monoLoaded) promptView->getRenderer()->setFont(monoFont);
    promptView->getRenderer()->setBackgroundColor(tgui::Color(30, 30, 40));
    promptView->getRenderer()->setTextColor(tgui::Color(220, 220, 180));
    promptView->getRenderer()->setBorderColor(tgui::Color(80, 80, 100));
    panel->add(promptView);

    // Instruction label explaining what to do
    auto instructionLabel = tgui::Label::create("Type the corrected code below (Tab inserts indentation):");
    instructionLabel->setPosition({"3%", "50%"});
    instructionLabel->setTextSize(14);
    instructionLabel->getRenderer()->setTextColor(tgui::Color(180, 180, 180));
    panel->add(instructionLabel);

    auto editor = tgui::TextArea::create();
    editor->setPosition({"3%", "54%"});
    editor->setSize({"94%", "28%"});
    // Start with blank editor so player must type the correction
    editor->setText(prompt);
    editor->setTextSize(22);
    if (monoLoaded) editor->getRenderer()->setFont(monoFont);
    editor->getRenderer()->setBackgroundColor(tgui::Color(20, 20, 30));
    editor->getRenderer()->setTextColor(tgui::Color(200, 230, 200));
    editor->getRenderer()->setCaretColor(tgui::Color::White);
    editor->getRenderer()->setSelectedTextBackgroundColor(tgui::Color(60, 80, 140));
    editor->getRenderer()->setBorderColor(tgui::Color(80, 80, 100));
    panel->add(editor);
    editor->setFocused(true);

    auto submitButton = tgui::Button::create("Submit");
    submitButton->setPosition({"67%", "86%"});
    submitButton->setSize({"14%", "10%"});
    submitButton->getRenderer()->setBackgroundColor(tgui::Color(60, 120, 60));
    submitButton->getRenderer()->setTextColor(tgui::Color::White);
    panel->add(submitButton);

    auto cancelButton = tgui::Button::create("Cancel");
    cancelButton->setPosition({"82%", "86%"});
    cancelButton->setSize({"14%", "10%"});
    cancelButton->getRenderer()->setBackgroundColor(tgui::Color(120, 60, 60));
    cancelButton->getRenderer()->setTextColor(tgui::Color::White);
    panel->add(cancelButton);

    const std::string hintText = keywordHint.empty()
        ? "No required keyword for this challenge."
        : ("Required keyword: " + keywordHint);
    auto hint = tgui::Label::create(hintText);
    hint->setPosition({"3%", "87%"});
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

    cancelButton->onPress([&]() { popup.close(); });

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
}

BattleState::BattleState(sf::RenderWindow& window)
: mWindow(window)
, mDummyTexture(makeDummyTexture())
, mEnemySprite(mDummyTexture)
, mPlayerSprite(mDummyTexture) {
    const auto ws = mWindow.getSize();
    mBattleView = sf::View(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(ws.x), static_cast<float>(ws.y)}
    ));

    mBackground.setPosition({0.f, 0.f});
    mBackground.setSize({static_cast<float>(ws.x), static_cast<float>(ws.y)});
    mBackground.setFillColor(sf::Color(64, 0, 128));

    const std::vector<std::string> fontCandidates = {
        "assets/fonts/arial.ttf",
        "../assets/fonts/arial.ttf",
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
    mBattleMessage = "Press E or F1 to open Debug Editor. ESC = exit.";
#else
    mBattleMessage = "Press Enter to submit fix. Backspace = wrong attempt. ESC = exit.";
#endif

    mEnemyPlatform.setFillColor(sf::Color(110, 90, 150, 180));
    mPlayerPlatform.setFillColor(sf::Color(110, 90, 150, 180));

    mEnemySpriteLoaded = loadEncounterSprite();
    mPlayerSpriteLoaded = loadPlayerSprite();
    updateLayout();

    std::cout << "[BattleState] Initialized with encounter sprite: " << mEnemySpritePath << "\n";
}

bool BattleState::loadEncounterSprite() {
    mEnemySpritePath = chooseRandomMonsterAsset();
    if (mEnemySpritePath.empty()) {
        return false;
    }

    const std::string srcPath = "src/assets/" + mEnemySpritePath;
    const std::string assetsPath = "assets/" + mEnemySpritePath;
    const std::string parentSrcPath = "../src/assets/" + mEnemySpritePath;
    const std::string parentAssetsPath = "../assets/" + mEnemySpritePath;

    if (!loadTextureFromAny(mEnemyTexture, {
        srcPath.c_str(), assetsPath.c_str(), parentSrcPath.c_str(), parentAssetsPath.c_str()
    })) {
        std::cerr << "WARNING: Failed to load encounter sprite: " << mEnemySpritePath << "\n";
        return false;
    }

    mEnemySprite.setTexture(mEnemyTexture, true);
    return true;
}

bool BattleState::loadPlayerSprite() {
    if (!loadTextureFromAny(mPlayerTexture, {
        "src/assets/forest_mon.png",
        "assets/forest_mon.png",
        "../src/assets/forest_mon.png",
        "../assets/forest_mon.png"
    })) {
        std::cerr << "WARNING: Failed to load player battle sprite.\n";
        return false;
    }

    mPlayerSprite.setTexture(mPlayerTexture, true);
    return true;
}

void BattleState::updateLayout() {
    const float width = static_cast<float>(mWindow.getSize().x);
    const float height = static_cast<float>(mWindow.getSize().y);

    mBackground.setSize({width, height});

    mEnemyPlatform.setSize({width * 0.27f, height * 0.08f});
    mEnemyPlatform.setPosition({width * 0.60f, height * 0.24f});

    mPlayerPlatform.setSize({width * 0.31f, height * 0.10f});
    mPlayerPlatform.setPosition({width * 0.10f, height * 0.70f});

    if (mEnemySpriteLoaded) {
#if SFML_VERSION_MAJOR >= 3
        const auto bounds = mEnemySprite.getLocalBounds();
        if (bounds.size.x > 0.f && bounds.size.y > 0.f) {
            const float targetHeight = height * (mEncounterIntroActive ? 0.46f : 0.30f);
            const float scale = targetHeight / bounds.size.y;
            mEnemySprite.setScale({scale, scale});
            const auto scaled = mEnemySprite.getGlobalBounds();
            const float x = mEncounterIntroActive ? (width - scaled.size.x) * 0.5f : width * 0.68f - scaled.size.x * 0.5f;
            const float y = mEncounterIntroActive ? height * 0.20f : height * 0.09f;
            mEnemySprite.setPosition({x, y});
        }
#else
        const auto bounds = mEnemySprite.getLocalBounds();
        if (bounds.width > 0.f && bounds.height > 0.f) {
            const float targetHeight = height * (mEncounterIntroActive ? 0.46f : 0.30f);
            const float scale = targetHeight / bounds.height;
            mEnemySprite.setScale(scale, scale);
            const auto scaled = mEnemySprite.getGlobalBounds();
            const float x = mEncounterIntroActive ? (width - scaled.width) * 0.5f : width * 0.68f - scaled.width * 0.5f;
            const float y = mEncounterIntroActive ? height * 0.20f : height * 0.09f;
            mEnemySprite.setPosition(x, y);
        }
#endif
    }

    if (mPlayerSpriteLoaded) {
#if SFML_VERSION_MAJOR >= 3
        const auto bounds = mPlayerSprite.getLocalBounds();
        if (bounds.size.x > 0.f && bounds.size.y > 0.f) {
            const float targetHeight = height * 0.34f;
            const float scale = targetHeight / bounds.size.y;
            mPlayerSprite.setScale({scale, scale});
            const auto scaled = mPlayerSprite.getGlobalBounds();
            mPlayerSprite.setPosition({width * 0.22f - scaled.size.x * 0.5f, height * 0.42f});
        }
#else
        const auto bounds = mPlayerSprite.getLocalBounds();
        if (bounds.width > 0.f && bounds.height > 0.f) {
            const float targetHeight = height * 0.34f;
            const float scale = targetHeight / bounds.height;
            mPlayerSprite.setScale(scale, scale);
            const auto scaled = mPlayerSprite.getGlobalBounds();
            mPlayerSprite.setPosition(width * 0.22f - scaled.width * 0.5f, height * 0.42f);
        }
#endif
    }
}

void BattleState::handleEvent(const sf::Event& e) {
#if SFML_VERSION_MAJOR >= 3
    if (const auto* resized = e.getIf<sf::Event::Resized>()) {
        mBattleView.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
        updateLayout();
    }

    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            mExitRequested = true;
            requestPop();
            return;
        }

        if (mEncounterIntroActive) {
            return;
        }

#if CODEMON_HAS_TGUI
        if ((keyPressed->code == sf::Keyboard::Key::E || keyPressed->code == sf::Keyboard::Key::F1) &&
            mDebugEngine.hasActiveChallenge()) {
            const auto submission = runDebugEditorPopup(mDebugEngine.currentPrompt(), mCurrentKeywordHint);
            if (submission.has_value()) {
                applySubmission(mDebugEngine, submission.value(), mBattleMessage, mChallengeSolved, mSubmissionFailed);
            }
        }
#else
        if (keyPressed->code == sf::Keyboard::Key::Enter && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "return 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
        if (keyPressed->code == sf::Keyboard::Key::Backspace && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "retun 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
#endif
    }
#else
    if (e.type == sf::Event::Resized) {
        mBattleView.setSize({static_cast<float>(e.size.width), static_cast<float>(e.size.height)});
        updateLayout();
    }

    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Escape) {
            mExitRequested = true;
            requestPop();
            return;
        }

        if (mEncounterIntroActive) {
            return;
        }

#if CODEMON_HAS_TGUI
        if ((e.key.code == sf::Keyboard::E || e.key.code == sf::Keyboard::F1) &&
            mDebugEngine.hasActiveChallenge()) {
            const auto submission = runDebugEditorPopup(mDebugEngine.currentPrompt(), mCurrentKeywordHint);
            if (submission.has_value()) {
                applySubmission(mDebugEngine, submission.value(), mBattleMessage, mChallengeSolved, mSubmissionFailed);
            }
        }
#else
        if (e.key.code == sf::Keyboard::Return && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "return 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
        if (e.key.code == sf::Keyboard::BackSpace && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "retun 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
#endif
    }
#endif
}

void BattleState::update(sf::Time dt) {
    mTimer += dt.asSeconds();

    if (mEncounterIntroActive) {
        mEncounterIntroTimer += dt.asSeconds();
        if (mEncounterIntroTimer >= kEncounterIntroDuration) {
            mEncounterIntroActive = false;
            updateLayout();
        }
    }
}

void BattleState::render(sf::RenderTarget& target) {
    target.setView(mBattleView);

    if (mChallengeSolved) {
        mBackground.setFillColor(sf::Color(20, 120, 40));
    } else if (mSubmissionFailed) {
        mBackground.setFillColor(sf::Color(150, 0, 0));
    } else {
        mBackground.setFillColor(sf::Color(64, 0, 128));
    }

    target.draw(mBackground);

    if (mEncounterIntroActive) {
        if (mEnemySpriteLoaded) {
            target.draw(mEnemySprite);
        }

        if (mFontLoaded) {
#if SFML_VERSION_MAJOR >= 3
            sf::Text intro(mFont, "A wild Codemon appeared!", 34);
#else
            sf::Text intro("A wild Codemon appeared!", mFont, 34);
#endif
            intro.setFillColor(sf::Color::White);
#if SFML_VERSION_MAJOR >= 3
            const auto bounds = intro.getLocalBounds();
            intro.setOrigin({bounds.size.x * 0.5f, bounds.size.y * 0.5f});
#else
            const auto bounds = intro.getLocalBounds();
            intro.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
#endif
            intro.setPosition({static_cast<float>(mWindow.getSize().x) * 0.5f,
                               static_cast<float>(mWindow.getSize().y) * 0.10f});
            target.draw(intro);
        }
        return;
    }

    target.draw(mEnemyPlatform);
    target.draw(mPlayerPlatform);

    if (mEnemySpriteLoaded) {
        target.draw(mEnemySprite);
    }
    if (mPlayerSpriteLoaded) {
        target.draw(mPlayerSprite);
    }

    if (!mFontLoaded) {
        return;
    }

#if SFML_VERSION_MAJOR >= 3
    sf::Text title(mFont, "BATTLE DEBUG", 40);
    sf::Text subtitle(mFont, mBattleMessage, 22);
#else
    sf::Text title("BATTLE DEBUG", mFont, 40);
    sf::Text subtitle(mBattleMessage, mFont, 22);
#endif
    title.setFillColor(sf::Color::White);
    subtitle.setFillColor(sf::Color(230, 230, 230));

    const std::string promptLine = mDebugEngine.hasActiveChallenge()
        ? "Can you find what's wrong with the following code?"
        : std::string("Challenge solved. Press ESC to return.");

#if CODEMON_HAS_TGUI
    const std::string controlsLine = "Controls: E/F1 open editor, ESC exits battle";
#else
    const std::string controlsLine = "Controls: Enter=correct test, Backspace=wrong test, ESC exits";
#endif

#if SFML_VERSION_MAJOR >= 3
    sf::Text prompt(mFont, promptLine, 20);
    sf::Text controls(mFont, controlsLine, 18);
#else
    sf::Text prompt(promptLine, mFont, 20);
    sf::Text controls(controlsLine, mFont, 18);
#endif

    prompt.setFillColor(sf::Color(220, 220, 140));
    controls.setFillColor(sf::Color(200, 200, 200));

#if SFML_VERSION_MAJOR >= 3
    const auto bounds = title.getLocalBounds();
    title.setOrigin({bounds.size.x * 0.5f, bounds.size.y * 0.5f});
#else
    const auto bounds = title.getLocalBounds();
    title.setOrigin({bounds.width * 0.5f, bounds.height * 0.5f});
#endif
    title.setPosition({static_cast<float>(mWindow.getSize().x) * 0.5f,
                       static_cast<float>(mWindow.getSize().y) * 0.08f});

    subtitle.setPosition({40.f, static_cast<float>(mWindow.getSize().y) * 0.84f});
    prompt.setPosition({40.f, static_cast<float>(mWindow.getSize().y) * 0.90f});
    controls.setPosition({40.f, static_cast<float>(mWindow.getSize().y) * 0.95f});

    const float pulse = 0.5f + 0.5f * std::sin(mTimer * 3.f);
    const auto alpha = static_cast<std::uint8_t>(128 + 127 * pulse);
    title.setFillColor(sf::Color(255, 255, 255, alpha));

    target.draw(title);
    target.draw(subtitle);
    target.draw(prompt);
    target.draw(controls);
}
