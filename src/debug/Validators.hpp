#pragma once
#include <string>
#include <algorithm>
#include <cctype>

/**
 * @namespace Validators
 * @brief Contains logic for verifying if player-submitted code fixes are correct.
 * * This namespace acts as the "Judgment Engine" for the game. When a player
 * interacts with a bug in the world or a battle, their string input is passed
 * here to determine if the logic error has been resolved.
 */
namespace Validators {

    /**
     * @brief Normalizes strings by removing whitespace and converting to lowercase.
     * @param s The raw string to be cleaned.
     * @return A "flat" version of the string for easy comparison.
     */
    inline std::string sanitize(std::string s) {
        // Remove all whitespace (spaces, tabs, newlines)
        s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c) {
            return std::isspace(c);
        }), s.end());
        
        // Convert to lowercase so 'IF' and 'if' match
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        });
        return s;
    }

    /**
     * @brief Performs a normalized comparison between player input and a solution.
     * * By using sanitize(), "if ( X == 0 )" and "if(x==0)" are both treated as correct.
     * * @param input The raw string typed by the player in the UI.
     * @param solution The hardcoded "correct" version of the code.
     * @return true if the code is logically equivalent, false otherwise.
     */
    inline bool exactMatch(std::string input, std::string solution) {
        return sanitize(input) == sanitize(solution);
    }

    /**
     * @brief Checks if a specific "fix" keyword exists within the player's code.
     * Useful for challenges where the player must include a specific function or fix.
     */
    inline bool containsKeyword(const std::string& input, const std::string& keyword) {
        return sanitize(input).find(sanitize(keyword)) != std::string::npos;
    }
}