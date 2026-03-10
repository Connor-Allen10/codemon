#pragma once

#include <string>
#include "ValidationResult.hpp"

namespace Debug {

/**
 * @brief Represents a single debug challenge that the player can attempt.
 *
 * A challenge consists of a prompt shown to the user and a canonical solution.
 * The `validate` member returns a ValidationResult describing whether the
 * submission is acceptable.  The class can also be used during development
 * to force a victory (so states can skip input).
 */
struct Challenge {
    // ---------------------------------------------------------------------
    // public data - these are deliberately simple/plain-old-data so that
    // states can construct challenges on the fly or deserialize them from
    // JSON without needing a bespoke builder.
    // ---------------------------------------------------------------------

    std::string prompt;         ///< text shown in the debug overlay
    std::string solution;       ///< expected corrected code string
    std::string keywordHint;    ///< optional token the submission must contain

    // -----------------------------------------------------------------
    // construction / validation helpers
    // -----------------------------------------------------------------

    /**
     * @brief Construct a challenge with the given prompt and solution.
     *
     * @param p      Description presented to the player.
     * @param sol    The canonical fixed code snippet.
     * @param hint   If non‑empty, submissions must include this keyword.
     */
    Challenge(std::string p,
              std::string sol,
              std::string hint = "");

    /**
     * @brief Check a player's submission and return a ValidationResult.
     *
     * The result contains a boolean success flag and a human-readable
     * feedback message suitable for direct display in the UI.
     */
    ValidationResult validate(const std::string& submission) const;

    /**
     * @brief During development, force this challenge to succeed regardless of
     * actual submission contents.
     */
    void forceVictory(bool v) { victoryForced = v; }

private:
    // internal flag used by forceVictory(); not serialized or displayed.
    bool victoryForced = false;
};

/**
 * @brief Minimal core debug engine that manages one active challenge.
 *
 * This is intentionally UI-agnostic and can be reused by BattleState,
 * DebugState, and future TGUI views.
 */
class Engine {
public:
    /// Load a new active challenge (replaces previous one if any).
    void startChallenge(Challenge challenge) {
        mActive = std::move(challenge);
        mHasActive = true;
        mLastResult = ValidationResult{};
    }

    /// Whether there is a challenge currently active.
    bool hasActiveChallenge() const { return mHasActive; }

    /// Prompt for the active challenge, or empty string if inactive.
    std::string currentPrompt() const {
        return mHasActive ? mActive.prompt : std::string{};
    }

    /// Submit an answer to the active challenge.
    /// Returns failure if no challenge is active.
    ValidationResult submit(const std::string& submission) {
        if (!mHasActive) {
            mLastResult = {false, "No active challenge.", 0.0f};
            return mLastResult;
        }

        mLastResult = mActive.validate(submission);
        if (mLastResult.success) {
            mHasActive = false;
        }
        return mLastResult;
    }

    /// Last validation result returned by submit().
    const ValidationResult& lastResult() const { return mLastResult; }

    /// Clear active challenge and cached result.
    void reset() {
        mHasActive = false;
        mLastResult = ValidationResult{};
    }

private:
    Challenge mActive{"", "", ""};
    bool mHasActive = false;
    ValidationResult mLastResult{};
};

} // namespace Debug
