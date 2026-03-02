#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class StateStack; // Forward declaration

// Action requests that states can make to the StateStack
enum class StateAction {
    None,
    Push,
    Pop
};

class State {
public:
    virtual ~State() = default;
    virtual void handleEvent(const sf::Event& e) = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void render(sf::RenderTarget& target) = 0;

    // States can request actions on the stack
    StateAction getPendingAction() const { return mPendingAction; }
    std::unique_ptr<State> takePendingState() { return std::move(mPendingState); }
    void clearPendingAction() { mPendingAction = StateAction::None; }

protected:
    void requestPush(std::unique_ptr<State> state) {
        mPendingAction = StateAction::Push;
        mPendingState = std::move(state);
    }

    void requestPop() {
        mPendingAction = StateAction::Pop;
    }

private:
    StateAction mPendingAction = StateAction::None;
    std::unique_ptr<State> mPendingState = nullptr;
};
