/**
 * @file BattleState.cpp
 * @brief Implementation of the BattleState class.
 * 
 * This file implements battle encounters integrated with the Debug::Engine system.
 * The battle state presents coding challenges that players must solve by submitting
 * correct code fixes. Key features:
 * - Debug::Engine manages challenge lifecycle (start, submit, validation)
 * - Enter key submits correct answer for testing
 * - Backspace key submits incorrect answer to test failure path
 * - Visual feedback changes background color on success
 * - Challenge prompt and feedback messages displayed via SFML text rendering
 */

#include "BattleState.hpp"

#include <cmath>
#include <filesystem>
#include <vector>
#include <iostream>

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

    // ===== Debug::Engine Integration =====
    // Start a hardcoded debug challenge for Step 2 testing.
    // Challenge: Fix typo in C++ return statement ("retun" -> "return")
    // - Prompt: "Fix bug: change 'retun 0;' to valid C++"
    // - Expected answer: "return 0;"
    // - Required keyword: "return" (must appear in submission)
    // This will later be replaced with dynamic challenge loading in Step 3.
    mDebugEngine.startChallenge(Debug::Challenge{
        "Fix bug: change 'retun 0;' to valid C++",
        "return 0;",
        "return"
    });
    mBattleMessage = "Battle started. Press Enter to submit fix. Backspace = wrong attempt. ESC = exit.";

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

        // Enter key: Submit CORRECT answer to test success path
        // Submits "return 0;" which matches the expected answer.
        // On success: feedback = "Correct! ...", mChallengeSolved = true, background turns green
        if (keyPressed->code == sf::Keyboard::Key::Enter && mDebugEngine.hasActiveChallenge()) {
            const ValidationResult r = mDebugEngine.submit("return 0;");
            mBattleMessage = r.feedback;
            mChallengeSolved = r.success;
        }

        // Backspace key: Submit WRONG answer to test failure path
        // Submits "retun 0;" (typo) which doesn't match expected answer.
        // On failure: feedback = "Incorrect. ...", mChallengeSolved = false, challenge stays active
        if (keyPressed->code == sf::Keyboard::Key::Backspace && mDebugEngine.hasActiveChallenge()) {
            const ValidationResult r = mDebugEngine.submit("retun 0;");
            mBattleMessage = r.feedback;
            mChallengeSolved = r.success;
        }
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

        // Enter/Return key: Submit CORRECT answer ("return 0;")
        // Tests success path for Debug::Engine validation
        if (e.key.code == sf::Keyboard::Return && mDebugEngine.hasActiveChallenge()) {
            const ValidationResult r = mDebugEngine.submit("return 0;");
            mBattleMessage = r.feedback;
            mChallengeSolved = r.success;
        }

        // Backspace key: Submit WRONG answer ("retun 0;" with typo)
        // Tests failure path for Debug::Engine validation
        if (e.key.code == sf::Keyboard::BackSpace && mDebugEngine.hasActiveChallenge()) {
            const ValidationResult r = mDebugEngine.submit("retun 0;");
            mBattleMessage = r.feedback;
            mChallengeSolved = r.success;
        }
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
    // - Purple (64, 0, 128): Challenge active/unsolved (default battle color)
    if (mChallengeSolved) {
        mBackground.setFillColor(sf::Color(20, 120, 40));
    } else {
        mBackground.setFillColor(sf::Color(64, 0, 128));
    }

    target.draw(mBackground);
    
    // ===== Debug UI Text Rendering =====
    // Three-line layout for battle debug interface:
    // 1. Title: "BATTLE DEBUG" with pulsing alpha animation
    // 2. Subtitle: Feedback message (mBattleMessage) showing validation result
    // 3. Prompt: Current challenge prompt or completion message
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

    // Prompt line shows active challenge or completion message
    std::string promptLine = mDebugEngine.hasActiveChallenge()
        ? ("Challenge: " + mDebugEngine.currentPrompt())
        : std::string("Challenge solved. Press ESC to return.");

#if SFML_VERSION_MAJOR >= 3
    sf::Text prompt(mFont, promptLine, 20);
#else
    sf::Text prompt(promptLine, mFont, 20);
#endif
    prompt.setFillColor(sf::Color(220, 220, 140));
        
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
        prompt.setPosition({40.f, static_cast<float>(mWindow.getSize().y) * 0.65f});
        
        // Apply pulse effect to alpha channel (fade in/out)
        const float pulse = 0.5f + 0.5f * std::sin(mTimer * 3.f);
        const auto alpha = static_cast<std::uint8_t>(128 + 127 * pulse);
        title.setFillColor(sf::Color(255, 255, 255, alpha));

        target.draw(title);
        target.draw(subtitle);
        target.draw(prompt);
    }
}
