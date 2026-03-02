/**
 * @file StateStack.hpp
 * @brief Manages a stack of game states with push/pop operations.
 * 
 * The StateStack maintains a vector of states where only the top state
 * is active (receives events, updates, and renders). States can request
 * to push new states or pop themselves via the State action system.
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "State.hpp"

/**
 * @class StateStack
 * @brief Stack-based state manager for the game.
 * 
 * The StateStack implements a simple state machine where:
 * - Only the top state is active
 * - States can push new states (pause current, run new)
 * - States can pop themselves (destroy current, resume previous)
 * 
 * Example flow:
 * 1. Start with WorldState
 * 2. Encounter triggers -> WorldState pushes BattleState
 * 3. Battle ends -> BattleState pops itself
 * 4. WorldState resumes
 * 
 * The stack processes state action requests after each update() and
 * handleEvent() call to ensure clean state transitions.
 */
class StateStack {
public:
    /**
     * @brief Push a new state onto the stack.
     * @param state The state to push (takes ownership)
     * 
     * The new state becomes active immediately. The previous top state
     * is paused but remains in the stack.
     */
    void push(std::unique_ptr<State> state);
    
    /**
     * @brief Pop the top state from the stack.
     * 
     * The top state is destroyed. If there are remaining states,
     * the new top state becomes active.
     */
    void pop();

    /**
     * @brief Handle input events for the top state.
     * @param e The SFML event to process
     * 
     * Only the top (active) state receives events. After handling,
     * any pending state actions are processed.
     */
    void handleEvent(const sf::Event& e);
    
    /**
     * @brief Update the top state's logic.
     * @param dt Delta time since last update
     * 
     * Only the top (active) state is updated. After updating,
     * any pending state actions are processed.
     */
    void update(sf::Time dt);
    
    /**
     * @brief Render the top state's visuals.
     * 
     * Only the top (active) state is rendered. Requires a render
     * target to be set via setRenderTarget().
     */
    void render();

    /**
     * @brief Set the render target for state rendering.
     * @param target Pointer to the render target (usually a window)
     */
    void setRenderTarget(sf::RenderTarget* target);

    /**
     * @brief Check if the stack is empty.
     * @return true if no states are in the stack
     */
    bool isEmpty() const { return mStack.empty(); }
    
    /**
     * @brief Get the number of states in the stack.
     * @return Number of states currently in the stack
     */
    size_t size() const { return mStack.size(); }

private:
    /**
     * @brief Process any pending push/pop actions from the top state.
     * 
     * Called after handleEvent() and update() to ensure state transitions
     * happen at safe points. Prevents mid-update state changes.
     */
    void processPendingActions();

private:
    std::vector<std::unique_ptr<State>> mStack;   ///< Stack of states (back = top)
    sf::RenderTarget* mTarget = nullptr;          ///< Render target for states
};
