#include "StateStack.hpp"

void StateStack::push(std::unique_ptr<State> state) {
    mStack.push_back(std::move(state));
}

void StateStack::pop() {
    if (!mStack.empty()) mStack.pop_back();
}

void StateStack::handleEvent(const sf::Event& e) {
    if (!mStack.empty()) {
        mStack.back()->handleEvent(e);
        processPendingActions();
    }
}

void StateStack::update(sf::Time dt) {
    if (!mStack.empty()) {
        mStack.back()->update(dt);
        processPendingActions();
    }
}

void StateStack::render() {
    if (!mTarget || mStack.empty()) return;
    mStack.back()->render(*mTarget);
}

void StateStack::setRenderTarget(sf::RenderTarget* target) {
    mTarget = target;
}

void StateStack::processPendingActions() {
    if (mStack.empty()) return;

    State* currentState = mStack.back().get();
    StateAction action = currentState->getPendingAction();

    if (action == StateAction::Push) {
        auto newState = currentState->takePendingState();
        currentState->clearPendingAction();
        if (newState) {
            push(std::move(newState));
        }
    } else if (action == StateAction::Pop) {
        currentState->clearPendingAction();
        pop();
    }
}
