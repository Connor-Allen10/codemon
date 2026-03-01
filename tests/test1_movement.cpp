#include <gtest/gtest.h>
#include "../src/states/WorldState.hpp"

class WorldStateTest : public ::testing::Test {
protected:
    sf::RenderWindow window{sf::VideoMode({800,600}), "test", sf::Style::None};
    WorldState world{window};
};

TEST_F(WorldStateTest, GameLoadsWithoutCrashing) {
    // Basic sanity test - WorldState initializes successfully
    // Real gameplay testing would require integration with Game loop
    EXPECT_TRUE(true);
}