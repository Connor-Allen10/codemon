#include <gtest/gtest.h>
#include "debug/DebugChallenge.hpp"
#include "debug/Validators.hpp"

using namespace Debug;
using namespace Validators;

// Integration test that exercises both challenge validation and a
// post-success battle check.
TEST(DebugIntegrationTest, ChallengeThenBattle) {
    Challenge c("Example", "return;", "");
    ValidationResult r = c.validate("return;");
    ASSERT_TRUE(r.success);

    ValidationResult b = validateBattleAction(5, 3, false);
    EXPECT_TRUE(b.success);
}

// Integration cases for battle action alone (reuse from validators edge tests)
TEST(DebugIntegrationTest, BattleFailsPP) {
    ValidationResult b = validateBattleAction(2, 10, false);
    EXPECT_FALSE(b.success);
}

TEST(DebugIntegrationTest, BattleFailsSilenced) {
    ValidationResult b = validateBattleAction(10, 1, true);
    EXPECT_FALSE(b.success);
}