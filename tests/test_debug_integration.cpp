#include <gtest/gtest.h>
#include "debug/DebugChallenge.hpp"
#include "debug/Validators.hpp"

using namespace Debug;
using namespace Validators;

// -----------------------------------------------------------------------------
// Integration tests for debug systems
//
// These tests validate behavior across module boundaries, making sure
// `Debug::Challenge` and validator helpers work together as expected.
// -----------------------------------------------------------------------------

// Happy-path integration:
// 1) The player submits a correct challenge answer.
// 2) The game then validates that a battle action is allowed.
//
// This ensures that success from challenge validation can flow directly into
// subsequent battle validation logic.
TEST(DebugIntegrationTest, ChallengeThenBattle) {
    Challenge c("Example", "return;", "");
    ValidationResult r = c.validate("return;");
    ASSERT_TRUE(r.success);

    ValidationResult b = validateBattleAction(5, 3, false);
    EXPECT_TRUE(b.success);
}

// Integration failure case:
// Battle action should fail when required PP exceeds current PP.
TEST(DebugIntegrationTest, BattleFailsPP) {
    ValidationResult b = validateBattleAction(2, 10, false);
    EXPECT_FALSE(b.success);
}

// Integration failure case:
// Battle action should fail when the unit is silenced,
// even if PP would otherwise be sufficient.
TEST(DebugIntegrationTest, BattleFailsSilenced) {
    ValidationResult b = validateBattleAction(10, 1, true);
    EXPECT_FALSE(b.success);
}