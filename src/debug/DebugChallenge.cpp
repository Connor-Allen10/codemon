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
    // Enforce keyword only when it is an actual token in the canonical (authorized)
    // solution. Some challenge files use KEYWORD as a human-readable hint
    // sentence, which should not be required verbatim in submissions.
    const bool enforceKeyword = !keywordHint.empty() &&
                                Validators::containsKeyword(solution, keywordHint);

    if (enforceKeyword && !Validators::containsKeyword(submission, keywordHint)) {
        return { false,
                 "Your fix must include '" + keywordHint + "'." };
    }
    if (Validators::exactMatch(submission, solution)) {
        return { true, "Correct - the bug is fixed!" };
    }

    return { false, "That doesn't look right; try again." };
}

} // namespace Debug
