#pragma once
#include <string>

/**
 * @struct ValidationResult
 * @brief A data package returned by the Validation Engine to the Game States.
 * * When the BattleState or WorldState asks a Validator to check code, they 
 * receive this struct back. It tells the UI what to display and the game 
 * whether to unlock the next area or deal damage to an enemy.
 */
struct ValidationResult {
    // True if the player successfully fixed the bug.
    bool success;

    // A message to be displayed in the Game's terminal/overlay.
    // Example: "Syntax Error: Missing semicolon at line 1."
    std::string feedback;

    // Optional: Could be used in BattleState to determine how much
    // damage a "Code Fix" attack does based on how clean the code is.
    float efficiencyRating = 1.0f; 
};