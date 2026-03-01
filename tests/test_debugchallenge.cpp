// test_debugchallenge.cpp
// Unit tests for the Debug::Challenge class.
//
// These tests verify that the challenge validation logic correctly:
// - Compares player submissions to expected solutions (case/whitespace insensitive)
// - Enforces optional keyword hints when specified
// - Supports developer-mode victory forcing for rapid iteration

#include <gtest/gtest.h>
#include "debug/DebugChallenge.hpp"

using namespace Debug;

// Test that exact matches (after normalization) are accepted.
// The input "int x = 0;" should match the solution "int x=0;" because
// both normalize to "intx=0;" (spaces removed, lowercased).
TEST(DebugChallengeTest, ValidateCorrectSubmission) {
    Challenge c("Fix bug", "int x=0;", "");
    ValidationResult r = c.validate("int x = 0;");
    EXPECT_TRUE(r.success);
}

// Test that keyword hints are enforced when specified.
// The first submission "bar();" lacks the required "foo" keyword,
// so it should fail. The second "foo();" contains the keyword and matches,
// so it should succeed.
TEST(DebugChallengeTest, ValidateKeywordHint) {
    Challenge c("Use foo", "foo();", "foo");
    ValidationResult r1 = c.validate("bar();");
    EXPECT_FALSE(r1.success);
    EXPECT_NE(r1.feedback.find("foo"), std::string::npos);
    ValidationResult r2 = c.validate("foo();");
    EXPECT_TRUE(r2.success);
}

// Test that the forceVictory flag allows developers to bypass validation.
// Useful for testing game flow without actually implementing challenge content.
TEST(DebugChallengeTest, ForceVictory) {
    Challenge c("any", "", "");
    c.forceVictory(true);
    ValidationResult r = c.validate("");
    EXPECT_TRUE(r.success);
}