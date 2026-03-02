#include <gtest/gtest.h>
#include "../src/game/StateStack.hpp"
#include "../src/game/State.hpp"
#include <SFML/Graphics.hpp>

// Mock state that requests a push after 1 update
class MockPushState : public State {
public:
    explicit MockPushState(bool shouldPush = true) : mShouldPush(shouldPush) {}
    
    void handleEvent(const sf::Event&) override {}
    
    void update(sf::Time) override {
        if (mShouldPush && !mHasPushed) {
            requestPush(std::make_unique<MockPushState>(false));
            mHasPushed = true;
        }
    }
    
    void render(sf::RenderTarget&) override {}
    
    bool hasPushed() const { return mHasPushed; }
    
private:
    bool mShouldPush = true;
    bool mHasPushed = false;
};

// Mock state that requests a pop after 1 update
class MockPopState : public State {
public:
    void handleEvent(const sf::Event&) override {}
    
    void update(sf::Time) override {
        if (!mHasPopped) {
            requestPop();
            mHasPopped = true;
        }
    }
    
    void render(sf::RenderTarget&) override {}
    
    bool hasPopped() const { return mHasPopped; }
    
private:
    bool mHasPopped = false;
};

// Test that push requests work correctly
TEST(StateTransitionTest, PushRequestAddsNewState) {
    StateStack stack;
    
    // Start with 1 state
    auto* rawPtr = new MockPushState(true);
    stack.push(std::unique_ptr<State>(rawPtr));
    EXPECT_EQ(stack.size(), 1);
    
    // Update should trigger push request
    stack.update(sf::seconds(0.016f));
    
    // Stack should now have 2 states
    EXPECT_EQ(stack.size(), 2);
    EXPECT_TRUE(rawPtr->hasPushed());
}

// Test that pop requests work correctly
TEST(StateTransitionTest, PopRequestRemovesState) {
    StateStack stack;
    
    // Start with 2 states (base + one that will pop)
    stack.push(std::make_unique<MockPushState>(false)); // Base state
    auto* popState = new MockPopState();
    stack.push(std::unique_ptr<State>(popState));
    EXPECT_EQ(stack.size(), 2);
    
    // Update should trigger pop request
    stack.update(sf::seconds(0.016f));
    
    // Stack should now have 1 state
    EXPECT_EQ(stack.size(), 1);
}

// Test push then pop sequence
TEST(StateTransitionTest, PushThenPopSequence) {
    StateStack stack;
    
    // Start with base state
    stack.push(std::make_unique<MockPushState>(false));
    EXPECT_EQ(stack.size(), 1);
    
    // Push a state that will pop itself
    stack.push(std::make_unique<MockPopState>());
    EXPECT_EQ(stack.size(), 2);
    
    // Update should trigger pop
    stack.update(sf::seconds(0.016f));
    
    // Back to 1 state
    EXPECT_EQ(stack.size(), 1);
}

// Test multiple pushes in sequence
TEST(StateTransitionTest, MultiplePushesInSequence) {
    StateStack stack;
    
    // Start with base state
    stack.push(std::make_unique<MockPushState>(false));
    EXPECT_EQ(stack.size(), 1);
    
    // Push a state that will push another
    auto* pusher = new MockPushState(true);
    stack.push(std::unique_ptr<State>(pusher));
    EXPECT_EQ(stack.size(), 2);
    
    // First update: pusher pushes a new state
    stack.update(sf::seconds(0.016f));
    EXPECT_EQ(stack.size(), 3);
    
    // Second update: nothing happens (new state doesn't push)
    stack.update(sf::seconds(0.016f));
    EXPECT_EQ(stack.size(), 3);
}

// Test that empty stack doesn't crash
TEST(StateTransitionTest, EmptyStackSafe) {
    StateStack stack;
    EXPECT_TRUE(stack.isEmpty());
    EXPECT_EQ(stack.size(), 0);
    
    // These should not crash
    stack.update(sf::seconds(0.016f));
    
#if SFML_VERSION_MAJOR >= 3
    sf::Event event = sf::Event::KeyPressed{.code = sf::Keyboard::Key::Escape};
    stack.handleEvent(event);
#else
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    stack.handleEvent(event);
#endif
    
    stack.render();
}

// Test pop on single state results in empty stack
TEST(StateTransitionTest, PopLastStateResultsInEmpty) {
    StateStack stack;
    
    // Add a state that pops itself
    stack.push(std::make_unique<MockPopState>());
    EXPECT_EQ(stack.size(), 1);
    
    // Update should pop
    stack.update(sf::seconds(0.016f));
    
    // Stack should be empty
    EXPECT_TRUE(stack.isEmpty());
    EXPECT_EQ(stack.size(), 0);
}
