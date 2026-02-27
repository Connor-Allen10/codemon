#include <gtest/gtest.h>
#include "debug/DebugChallenge.hpp"

using namespace Debug;

TEST(DebugChallengeTest, ValidateCorrectSubmission) {
    Challenge c("Fix bug", "int x=0;", "");
    ValidationResult r = c.validate("int x = 0;");
    EXPECT_TRUE(r.success);
}

TEST(DebugChallengeTest, ValidateKeywordHint) {
    Challenge c("Use foo", "foo();", "foo");
    ValidationResult r1 = c.validate("bar();");
    EXPECT_FALSE(r1.success);
    EXPECT_NE(r1.feedback.find("foo"), std::string::npos);
    ValidationResult r2 = c.validate("foo();");
    EXPECT_TRUE(r2.success);
}

TEST(DebugChallengeTest, ForceVictory) {
    Challenge c("any", "", "");
    c.forceVictory(true);
    ValidationResult r = c.validate("");
    EXPECT_TRUE(r.success);
}