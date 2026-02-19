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
     * @brief Performs a normalized comparison between player input and a solution.
     * * To prevent the game from being frustrating, this function "normalizes" 
     * strings by removing all whitespace before comparing. This means 
     * "if(x==0)" and "if ( x == 0 )" are both treated as correct.
     * * @param input The raw string typed by the player in the UI.
     * @param solution The hardcoded "correct" version of the code.
     * @return true if the code is logically equivalent, false otherwise.
     */
    inline bool exactMatch(std::string input, std::string solution) {
        // Lambda function to strip all whitespace characters (spaces, tabs, newlines)
        auto normalize = [](std::string s) {
            s.erase(std::remove_if(s.begin(), s.end(), [](unsigned char c) {
                return std::isspace(c);
            }), s.end());
            return s;
        };

        return normalize(input) == normalize(solution);
    }

    /**
     * @brief Checks if a specific "fix" keyword exists within the player's code.
     * * Useful for challenges where the player can write multiple things, but 
     * must include a specific fix (like changing '=' to '==').
     */
    inline bool containsKeyword(const std::string& input, const std::string& keyword) {
        return input.find(keyword) != std::string::npos;
    }
}