#include <gtest/gtest.h>
#include "debug/Validators.hpp"

using namespace Validators;

// Edge cases for sanitize/compare functions
TEST(ValidatorsEdgeTest, EmptySanitize) {
    EXPECT_EQ(sanitize(""), "");
}

TEST(ValidatorsEdgeTest, ExactMatchDifferent) {
    EXPECT_FALSE(exactMatch("foo", "bar"));
}

TEST(ValidatorsEdgeTest, KeywordCaseInsensitive) {
    EXPECT_TRUE(containsKeyword("FooBar", "foobar"));
    EXPECT_FALSE(containsKeyword("FooBar", "baz"));
}

// Battle action validator tests
TEST(ValidatorsEdgeTest, BattleAction_NoPP) {
    ValidationResult r = validateBattleAction(1, 5, false);
    EXPECT_FALSE(r.success);
    EXPECT_NE(r.feedback.find("Insufficient PP"), std::string::npos);
}

TEST(ValidatorsEdgeTest, BattleAction_Silenced) {
    ValidationResult r = validateBattleAction(10, 5, true);
    EXPECT_FALSE(r.success);
    EXPECT_NE(r.feedback.find("Silenced"), std::string::npos);
}

TEST(ValidatorsEdgeTest, BattleAction_OK) {
    ValidationResult r = validateBattleAction(10, 5, false);
    EXPECT_TRUE(r.success);
}