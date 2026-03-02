/**
 * @file State.hpp
 * @brief Abstract base class for all game states and state action system.
 * 
 * This file defines the State interface that all game states must implement,
 * as well as the StateAction system that allows states to request push/pop
 * operations on the StateStack.
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class StateStack; // Forward declaration

/**
 * @enum StateAction
 * @brief Actions that states can request to be performed on the StateStack.
 * 
 * States use these actions to control the state stack without direct access.
 * Actions are processed after update() and handleEvent() calls.
 */
enum class StateAction {
    None,  ///< No action pending
    Push,  ///< Request to push a new state onto the stack
    Pop    ///< Request to pop current state from the stack
};

/**
 * @class State
 * @brief Abstract base class for all game states (world, battle, menu, etc.).
 * 
 * States represent different screens or modes in the game. Each state implements
 * the core game loop functions: handleEvent(), update(), and render().
 * 
 * States can request to push new states or pop themselves from the StateStack
 * using the requestPush() and requestPop() protected methods. These requests
 * are processed by the StateStack after update/event handling completes.
 * 
 * Example state lifecycle:
 * 1. State is pushed onto stack
 * 2. handleEvent() called for input
 * 3. update() called for logic
 * 4. render() called for drawing
 * 5. State requests pop or push if needed
 * 6. StateStack processes the request
 */
class State {
public:
    virtual ~State() = default;
    
    /**
     * @brief Handle user input events.
     * @param e The SFML event to process (keyboard, mouse, window events)
     */
    virtual void handleEvent(const sf::Event& e) = 0;
    
    /**
     * @brief Update state logic.
     * @param dt Delta time since last update (fixed timestep)
     */
    virtual void update(sf::Time dt) = 0;
    
    /**
     * @brief Render state visuals.
     * @param target The render target to draw to (usually a window)
     */
    virtual void render(sf::RenderTarget& target) = 0;

    /**
     * @brief Get the pending action requested by this state.
     * @return The StateAction requested (None, Push, or Pop)
     */
    StateAction getPendingAction() const { return mPendingAction; }
    
    /**
     * @brief Take ownership of the pending state (for Push actions).
     * @return The new state to push, or nullptr if no state pending
     */
    std::unique_ptr<State> takePendingState() { return std::move(mPendingState); }
    
    /**
     * @brief Clear any pending action request.
     */
    void clearPendingAction() { mPendingAction = StateAction::None; }

protected:
    /**
     * @brief Request a new state be pushed onto the stack.
     * @param state The new state to push (takes ownership)
     * 
     * The new state will become active and this state will be paused.
     * Example: WorldState pushes BattleState during encounter.
     */
    void requestPush(std::unique_ptr<State> state) {
        mPendingAction = StateAction::Push;
        mPendingState = std::move(state);
    }

    /**
     * @brief Request this state be popped from the stack.
     * 
     * This state will be destroyed and the previous state will resume.
     * Example: BattleState pops itself when battle ends.
     */
    void requestPop() {
        mPendingAction = StateAction::Pop;
    }

private:
    StateAction mPendingAction = StateAction::None;          ///< Pending stack action
    std::unique_ptr<State> mPendingState = nullptr;          ///< State to push (if Push action)
};
