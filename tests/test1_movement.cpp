#include <gtest/gtest.h>
#include "../src/states/WorldState.hpp"

class WorldStateTest : public ::testing::Test {
protected:
    sf::RenderWindow window{sf::VideoMode({800,600}), "test", sf::Style::None};
    WorldState world{window};
};

TEST_F(WorldStateTest, MoveUp) {
    auto move = world.computeMovementInput(true,false,false,false);
    world.applyMovement(move, sf::seconds(1.f));

    EXPECT_LT(world.mPlayer.getPosition().y, 100.f);
}