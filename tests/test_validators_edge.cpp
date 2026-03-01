#include <gtest/gtest.h>
#include "debug/Validators.hpp"

using namespace Validators;

// -----------------------------------------------------------------------------
// Edge-case tests for validator helpers
//
// These tests target small but important boundary scenarios that can be missed
// in normal happy-path testing.
// -----------------------------------------------------------------------------

// Empty input should remain empty after sanitization.
TEST(ValidatorsEdgeTest, EmptySanitize) {
    EXPECT_EQ(sanitize(""), "");
}

// Completely different strings should not be considered an exact match.
TEST(ValidatorsEdgeTest, ExactMatchDifferent) {
    EXPECT_FALSE(exactMatch("foo", "bar"));
}

// Keyword checks should be case-insensitive and only pass on actual matches.
TEST(ValidatorsEdgeTest, KeywordCaseInsensitive) {
    EXPECT_TRUE(containsKeyword("FooBar", "foobar"));
    EXPECT_FALSE(containsKeyword("FooBar", "baz"));
}

// Battle action should fail when current PP is below the cost.
// Also verifies feedback includes an actionable message.
TEST(ValidatorsEdgeTest, BattleAction_NoPP) {
    ValidationResult r = validateBattleAction(1, 5, false);
    EXPECT_FALSE(r.success);
    EXPECT_NE(r.feedback.find("Insufficient PP"), std::string::npos);
}

// Battle action should fail when silenced status is active,
// regardless of available PP.
TEST(ValidatorsEdgeTest, BattleAction_Silenced) {
    ValidationResult r = validateBattleAction(10, 5, true);
    EXPECT_FALSE(r.success);
    EXPECT_NE(r.feedback.find("Silenced"), std::string::npos);
}

// Battle action should succeed when PP is sufficient and not silenced.
TEST(ValidatorsEdgeTest, BattleAction_OK) {
    ValidationResult r = validateBattleAction(10, 5, false);
    EXPECT_TRUE(r.success);
}