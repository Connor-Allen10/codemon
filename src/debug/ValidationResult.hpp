#pragma once
#include <string>

/**
 * @file ValidationResult.hpp
 * @brief Defines the result type produced by validator functions.
 *
 * Validators (either free functions or members of a challenge class) return
 * this struct so that game states can uniformly react to a player's attempt.
 * The `success` field drives game logic, `feedback` is written to the
 * on‑screen terminal, and `efficiencyRating` can be used for scoring or
 * damage multipliers in battle.
 */

struct ValidationResult {
    /// whether the submitted fix was accepted by the validator
    bool success = false;

    /// human‑readable message displayed to the player
    std::string feedback;

    /// optional multiplier (defaults to 1.0); reserved for later use
    float efficiencyRating = 1.0f;

    /// defaulted for convenience when fields will be set later
    ValidationResult() = default;

    /**
     * @brief Convenience constructor with all fields.
     *
     * @param s  success flag
     * @param f  feedback text
     * @param e  efficiency multiplier (default 1.0)
     */
    ValidationResult(bool s, std::string f, float e = 1.0f)
        : success(s), feedback(std::move(f)), efficiencyRating(e) {}
};