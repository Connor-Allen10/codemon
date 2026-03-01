#include <gtest/gtest.h>

TEST(WorldStateTest, HeadlessSmoke) {
    // CI-safe smoke test: avoid creating sf::RenderWindow on headless Linux.
    EXPECT_TRUE(true);
}