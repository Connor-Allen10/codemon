#pragma once
#include <string>
#include <algorithm>
#include <cctype>
#include "ValidationResult.hpp"

/**
 * @file Validators.hpp
 * @brief String‑matching helpers and simple validation routines used by the
 * debug challenge system.
 *
 * All functions are inline so that they can be used without linking a
 * separate library.  They operate on player‑submitted code snippets and
 * normalize formatting to make comparisons more forgiving.
 */

namespace Validators {

/**
 * @brief Remove whitespace and convert to lowercase.
 *
 * This is the canonical "normalization" step used before comparing two code
 * snippets.  It ensures that differences in indentation, spacing, or case
 * don't cause a valid fix to be rejected.
 */
inline std::string sanitize(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c) {
        return std::isspace(c);
    }), s.end());
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

/**
 * @brief Compare two code strings after normalization.
 *
 * @param input     The player's submission.
 * @param solution  The canonical "correct" code.
 * @return true if the sanitized strings match exactly, false otherwise.
 */
inline bool exactMatch(std::string input, std::string solution) {
    return sanitize(std::move(input)) == sanitize(std::move(solution));
}

/**
 * @brief Check whether a keyword appears in the submission.
 *
 * Useful for challenges that require the player to add a specific
 * function name, variable, or keyword.  Both input and keyword are sanitized
 * so formatting/case differences are ignored.
 */
inline bool containsKeyword(const std::string& input, const std::string& keyword) {
    return sanitize(input).find(sanitize(keyword)) != std::string::npos;
}

/**
 * @brief Example of a more complex validation function used in battle logic.
 *
 * Confirm that an action has sufficient "PP" (power points) and isn't
 * prevented by a silence status.  This function returns a ValidationResult
 * appropriate for passing directly back to the calling state.
 */
inline ValidationResult validateBattleAction(int currentPP, int cost, bool isSilenced) {
    if (isSilenced) {
        return {false, "Error: System Silenced. Compiler unreachable.", 0.0f};
    }
    if (currentPP < cost) {
        return {false, "Error: Insufficient PP to execute script.", 0.0f};
    }
    return {true, "Logic Verified. Executing...", 1.0f};
}

} // namespace Validators
