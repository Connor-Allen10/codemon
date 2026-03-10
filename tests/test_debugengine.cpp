// test_debugengine.cpp
// Unit tests for the core Debug::Engine flow.
//
// These tests verify that the engine:
// - Starts and exposes an active challenge prompt
// - Rejects submissions when no challenge is active
// - Clears active challenge after a successful submission
// - Keeps challenge active after a failed submission

#include <gtest/gtest.h>
#include "debug/DebugChallenge.hpp"

using namespace Debug;

// Verifies that starting a challenge marks the engine as active
// and exposes the expected prompt to the UI layer.
TEST(DebugEngineTest, StartsChallengeAndExposesPrompt) {
    Engine e;
    e.startChallenge(Challenge{"Fix assignment", "x=1;", ""});

    EXPECT_TRUE(e.hasActiveChallenge());
    EXPECT_EQ(e.currentPrompt(), "Fix assignment");
}

// Verifies that submitting without an active challenge returns
// a failure result with a helpful error message.
TEST(DebugEngineTest, SubmitWithoutActiveChallengeFails) {
    Engine e;
    ValidationResult r = e.submit("anything");

    EXPECT_FALSE(r.success);
    EXPECT_NE(r.feedback.find("No active challenge"), std::string::npos);
}

// Verifies that a correct submission succeeds and that the engine
// clears the active challenge after completion.
TEST(DebugEngineTest, SuccessfulSubmissionCompletesChallenge) {
    Engine e;
    e.startChallenge(Challenge{"Fix", "foo();", "foo"});

    ValidationResult r = e.submit("foo();");
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(e.hasActiveChallenge());
}

// Verifies that an incorrect submission fails while keeping the
// challenge active so the player can try again.
TEST(DebugEngineTest, FailedSubmissionKeepsChallengeActive) {
    Engine e;
    e.startChallenge(Challenge{"Fix", "foo();", "foo"});

    ValidationResult r = e.submit("bar();");
    EXPECT_FALSE(r.success);
    EXPECT_TRUE(e.hasActiveChallenge());
}
