/**
 * @file BattleState.cpp
 * @brief Implementation of the BattleState class.
 * 
 * This file implements battle encounters integrated with the Debug::Engine system.
 * The battle state presents coding challenges that players must solve by submitting
 * correct code fixes. Key features:
 * - Debug::Engine manages challenge lifecycle (start, submit, validation)
 * - Step 3: TGUI popup editor (E/F1) captures user code and submits it
 * - Fallback mode (no TGUI): Enter/Backspace test the validation paths
 * - Visual feedback changes background color on success
 * - Challenge prompt and feedback messages displayed via SFML text rendering
 */

#include "BattleState.hpp"

#include <cmath>
#include <filesystem>
#include <vector>
#include <iostream>
#include <optional>

#if __has_include(<TGUI/TGUI.hpp>) && __has_include(<TGUI/Backend/SFML-Graphics.hpp>)
#define CODEMON_HAS_TGUI 1
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#else
#define CODEMON_HAS_TGUI 0
#endif

// Static challenge loader shared across all battle instances
// Attempts to load from "challenges.txt" in working directory, falls back to 8 defaults
Debug::ChallengeLoader BattleState::sChallengeLoader("challenges.txt");

namespace {
// Centralized helper so all submission paths (TGUI and fallback hotkeys)
// update BattleState UI fields in one place.
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
// UI: modal editor popup used during battle.
// Returns:
// - std::string submission when player presses Submit
// - std::nullopt when player closes/cancels popup
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

    // Header + prompt context so the player knows what to fix.
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
    editor->setText("");
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

    // Submit captures textbox content and exits the modal loop.
    submitButton->onPress([&]() {
        submission = editor->getText().toStdString();
        submitted = true;
        popup.close();
    });

    // Cancel just closes without changing challenge state.
    cancelButton->onPress([&]() {
        popup.close();
    });

    while (popup.isOpen()) {
#if SFML_VERSION_MAJOR >= 3
    // SFML 3 event model (std::optional from pollEvent).
        while (const std::optional event = popup.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                popup.close();
            }
            gui.handleEvent(*event);
        }
#else
    // SFML 2 event model.
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

BattleState::BattleState(sf::RenderWindow& window)
: mWindow(window) {
    // Initialize view to match window size
    const auto ws = mWindow.getSize();
    mBattleView = sf::View(sf::FloatRect(
        {0.f, 0.f},
        {static_cast<float>(ws.x), static_cast<float>(ws.y)}
    ));

    // Setup battle background (dark purple)
    mBackground.setPosition({0.f, 0.f});
    mBackground.setSize({static_cast<float>(ws.x), static_cast<float>(ws.y)});
    mBackground.setFillColor(sf::Color(64, 0, 128));

    // Try to load font without spamming failed open attempts.
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

    // ===== Debug::Engine Integration (Dynamic Loading) =====
    // Load random challenge from ChallengeLoader.
    // The loader reads from file if available, otherwise uses 8 hardcoded defaults.
    // Challenge selection is random on each battle start for variety.
    auto maybeChallenge = sChallengeLoader.getRandomChallenge();
    if (maybeChallenge) {
        mCurrentKeywordHint = maybeChallenge->keywordHint;
        mDebugEngine.startChallenge(*maybeChallenge);
    } else {
        // Fallback if loader completely fails (shouldn't happen with defaults)
        mCurrentKeywordHint = "return";
        mDebugEngine.startChallenge(Debug::Challenge{
            "Fix bug: change 'retun 0;' to valid C++",
            "return 0;",
            "return"
        });
    }
#if CODEMON_HAS_TGUI
    // Default hint path: user opens editor and types their own fix.
    mBattleMessage = "Battle started. Press E or F1 to open Debug Editor. ESC = exit.";
#else
    // Fallback path used when TGUI is unavailable on the platform/build.
    mBattleMessage = "Battle started. Press Enter to submit fix. Backspace = wrong attempt. ESC = exit.";
#endif

    std::cout << "[BattleState] Initialized\n";
}

void BattleState::handleEvent(const sf::Event& e) {
#if SFML_VERSION_MAJOR >= 3
    if (const auto* keyPressed = e.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            mExitRequested = true;
            requestPop(); // Request to be removed from state stack
            std::cout << "[BattleState] Exit requested\n";
        }

#if CODEMON_HAS_TGUI
        // Open TGUI editor popup and submit the player's typed solution.
        if ((keyPressed->code == sf::Keyboard::Key::E || keyPressed->code == sf::Keyboard::Key::F1) &&
            mDebugEngine.hasActiveChallenge()) {
            const auto submission = runDebugEditorPopup(mDebugEngine.currentPrompt(), mCurrentKeywordHint);
            if (submission.has_value()) {
                applySubmission(mDebugEngine, submission.value(), mBattleMessage, mChallengeSolved, mSubmissionFailed);
            }
        }
#else
        // Enter key: Submit CORRECT answer to test success path
        // Submits "return 0;" which matches the expected answer.
        // On success: feedback = "Correct! ...", mChallengeSolved = true, background turns green
        if (keyPressed->code == sf::Keyboard::Key::Enter && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "return 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }

        // Backspace key: Submit WRONG answer to test failure path
        // Submits "retun 0;" (typo) which doesn't match expected answer.
        // On failure: feedback = "Incorrect. ...", mChallengeSolved = false, challenge stays active
        if (keyPressed->code == sf::Keyboard::Key::Backspace && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "retun 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
#endif
    }
    if (const auto* resized = e.getIf<sf::Event::Resized>()) {
        mBattleView.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
        mBackground.setSize({static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
    }
#else
    if (e.type == sf::Event::KeyPressed) {
        if (e.key.code == sf::Keyboard::Escape) {
            mExitRequested = true;
            requestPop(); // Request to be removed from state stack
            std::cout << "[BattleState] Exit requested\n";
        }

#if CODEMON_HAS_TGUI
        // Open TGUI editor popup and submit the player's typed solution.
        if ((e.key.code == sf::Keyboard::E || e.key.code == sf::Keyboard::F1) &&
            mDebugEngine.hasActiveChallenge()) {
            const auto submission = runDebugEditorPopup(mDebugEngine.currentPrompt(), mCurrentKeywordHint);
            if (submission.has_value()) {
                applySubmission(mDebugEngine, submission.value(), mBattleMessage, mChallengeSolved, mSubmissionFailed);
            }
        }
#else
        // Enter/Return key: Submit CORRECT answer ("return 0;")
        // Tests success path for Debug::Engine validation
        if (e.key.code == sf::Keyboard::Return && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "return 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }

        // Backspace key: Submit WRONG answer ("retun 0;" with typo)
        // Tests failure path for Debug::Engine validation
        if (e.key.code == sf::Keyboard::BackSpace && mDebugEngine.hasActiveChallenge()) {
            applySubmission(mDebugEngine, "retun 0;", mBattleMessage, mChallengeSolved, mSubmissionFailed);
        }
#endif
    }
    if (e.type == sf::Event::Resized) {
        mBattleView.setSize({static_cast<float>(e.size.width), static_cast<float>(e.size.height)});
        mBackground.setSize({static_cast<float>(e.size.width), static_cast<float>(e.size.height)});
    }
#endif
}

void BattleState::update(sf::Time dt) {
    mTimer += dt.asSeconds();
}

void BattleState::render(sf::RenderTarget& target) {
    target.setView(mBattleView);

    // Visual feedback: Change background color based on challenge state
    // - Green (20, 120, 40): Challenge solved successfully
    // - Red (150, 0, 0): Last submission was incorrect
    // - Purple (64, 0, 128): Challenge active, no attempt yet (default battle color)
    if (mChallengeSolved) {
        mBackground.setFillColor(sf::Color(20, 120, 40));
    } else if (mSubmissionFailed) {
        mBackground.setFillColor(sf::Color(150, 0, 0));
    } else {
        mBackground.setFillColor(sf::Color(64, 0, 128));
    }

    target.draw(mBackground);
    
    // ===== Debug UI Text Rendering =====
    // Two-line layout for battle debug interface:
    // 1. Title: "BATTLE DEBUG" with pulsing alpha animation
    // 2. Subtitle: Feedback/instruction message (mBattleMessage)
    if (mFontLoaded) {
        // SFML 3.0 vs 2.x have different Text constructor parameter order
#if SFML_VERSION_MAJOR >= 3
    sf::Text title(mFont, "BATTLE DEBUG", 40);
    sf::Text subtitle(mFont, mBattleMessage, 22);
#else
    sf::Text title("BATTLE DEBUG", mFont, 40);
    sf::Text subtitle(mBattleMessage, mFont, 22);
#endif
    title.setFillColor(sf::Color::White);
    subtitle.setFillColor(sf::Color(230, 230, 230));

        // Center text on screen
#if SFML_VERSION_MAJOR >= 3
        const auto bounds = title.getLocalBounds();
        title.setOrigin({bounds.size.x * 0.5f, bounds.size.y * 0.5f});
#else
        const auto bounds = title.getLocalBounds();
        title.setOrigin({bounds.width * 0.5f, bounds.height * 0.5f});
#endif
        title.setPosition({static_cast<float>(mWindow.getSize().x) * 0.5f,
                   static_cast<float>(mWindow.getSize().y) * 0.35f});

        subtitle.setPosition({40.f, static_cast<float>(mWindow.getSize().y) * 0.55f});
        
        // Apply pulse effect to alpha channel (fade in/out)
        const float pulse = 0.5f + 0.5f * std::sin(mTimer * 3.f);
        const auto alpha = static_cast<std::uint8_t>(128 + 127 * pulse);
        title.setFillColor(sf::Color(255, 255, 255, alpha));

        target.draw(title);
        target.draw(subtitle);
    }
}
