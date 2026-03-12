// DebugChallenge.cpp
// Implements the Debug::Challenge type declared in DebugChallenge.hpp.
// The class is used by various game states to drive the in‑game "fix the
// code" puzzles.  The validate() method contains the core logic and is tested
// by unit tests.

#include "DebugChallenge.hpp"
#include "Validators.hpp"
#include <iostream>

namespace Debug {

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------

Challenge::Challenge(std::string p,
                     std::string sol,
                     std::string hint)
    : prompt(std::move(p)),
      solution(std::move(sol)),
      keywordHint(std::move(hint)),
      victoryForced(false)
{}

//------------------------------------------------------------------------------
// validate
//------------------------------------------------------------------------------
// Run the challenge against the provided submission string.  If the
// `victoryForced` flag is set (used during development), the method will
// short‑circuit and report success.  Otherwise it checks for an optional
// keyword hint and then does a sanitized comparison of the code text.
//
// Returns a ValidationResult containing the success flag and human‑readable
// feedback message.

ValidationResult Challenge::validate(const std::string& submission) const {
    if (victoryForced) {
        return { true, "Debug: victory forced by developer." };
    }
/*
    if (!keywordHint.empty() &&
        !Validators::containsKeyword(submission, keywordHint)) {
        return { false,
                 "Your fix must include '" + keywordHint + "'." };
    }
*/
    if (Validators::exactMatch(submission, solution)) {
        return { true, "Correct - the bug is fixed!" };
    }

    return { false, "That doesn't look right; try again." };
}

} // namespace Debug
