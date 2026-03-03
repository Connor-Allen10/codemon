# Codémon: A Buggy World - Project Design Document

> **Note:** This is the living design document. For the most up-to-date information about building and running the game, please refer to the [README](../README.md).

## Executive Summary

**Codémon** is a Pokémon-style role-playing game designed to teach one of the most critical yet under-practiced programming skills: **debugging**. Instead of writing code from scratch, players progress through the game by identifying and fixing bugs embedded directly into the world, battles, and creatures. By reframing debugging as the core mechanic rather than a punishment, Codémon offers an engaging, low-pressure way for students to build confidence, pattern recognition, and problem-solving skills essential to real-world software development.

---

## Team & Communication

**Team Members:**
- **Connor Allen** – Project Manager (sprint planning, task tracking)
- **Isaac Hutchison** – UI/UX Designer (visual interaction, system design, state machine)
- **Lon Danna** – Software Developer & Test Engineer (game logic, testing infrastructure)

**Repository:** https://github.com/Connor-Allen10/codemon  
**Communication:** Discord  
**Version Control:** GitHub (feature branches + pull requests)

---

## Project Goals

**Primary Goal:** Help users practice and improve debugging skills in a fun, gamified environment that emphasizes understanding program behavior, spotting errors, and fixing bugs efficiently.

**Novelty:** Make debugging the main gameplay mechanic rather than a failure state:
- The game world itself is "buggy"
- Enemies behave incorrectly due to code errors
- Progression depends on diagnosing and fixing bugs

**Impact:**
- Reduce debugging anxiety
- Help students develop faster bug-recognition skills
- Serve as a supplementary learning tool for CS students
- Provide realistic representation of professional programming work

---

## Functional Requirements: Use Cases

### Use Case 1: Debugging Battles

**Actors:** Player  
**Trigger:** Player encounters a Codémon with broken behavior

**Success Scenario:**
1. Player encounters a wild Codémon or Trainer
2. Battle begins; player selects an attack
3. Player observes the attack has no effect or behaves incorrectly
4. Player opens the Codémon's source code interface
5. Player identifies the logic error in the code snippet
6. Player modifies the code and submits the fix
7. System validates the fix
8. Player returns to battle and the Codémon behaves correctly
9. Battle concludes

**Extensions:**
- **Optimization Bonus:** Fix using fewer lines of code → Critical Hit bonus
- **Hint System:** After 2 failed attempts, use "Scan" item to highlight the error line

**Failure Scenarios:**
- **Invalid Syntax:** Compiler error, Codémon skips its turn
- **Incorrect Logic:** Syntactically correct but doesn't solve the bug → try again next turn

---

### Use Case 2: World Progression Through Debugging

**Actors:** Player  
**Trigger:** Player encounters a blocked path or environmental obstacle

**Success Scenario:**
1. Player explores world and encounters a non-functional bridge
2. Player interacts with the bridge control panel
3. Code editor window appears showing BridgeControl logic
4. Player identifies a boolean error (e.g., `if (isPowered == false)`)
5. Player corrects the logic and saves
6. System validates the fix
7. Bridge visually extends
8. Player proceeds into newly unlocked area

**Extensions:**
- **Secret Paths:** Optional obstacles with more complex bugs lead to rare items

**Failure Scenarios:**
- **Logic Deadlock:** Code creates infinite loop → system prevents save
- **Out of Scope:** Player attempts to edit outside editable blocks → prevented

---

### Use Case 3: Codémon Growth & Evolution

**Actors:** Player  
**Trigger:** Codémon reaches evolution level but evolution isn't triggered

**Success Scenario:**
1. Player opens Codémon menu and selects a specific Codémon
2. Player notices "Evolve" button is greyed out despite level requirements
3. Player selects "Inspect Logic"
4. System displays evolution check: `if (level > 100)`
5. Player identifies 100 is an incorrect constant
6. Player changes value to 10 and submits
7. "Evolve" button becomes active
8. Evolution completes

**Extensions:**
- **Branching Evolution:** Change variable type to choose evolution form

**Failure Scenarios:**
- **Input Mismatch:** Non-numeric string entered → error message
- **Value Overflow:** Impossible value (e.g., -5) → system rejects

---

## Non-Functional Requirements

**Usability:**
- Debugging interfaces must be intuitive for beginners
- Clear visual feedback for correct/incorrect fixes

**Performance:**
- Game must run smoothly on standard student laptops
- Low latency for input and rendering

**Maintainability:**
- Modular game and debugging systems
- Clear separation between gameplay and bug logic

---

## External Requirements

**Error Handling:** Robust input validation. Game should not crash on unexpected input.

**Installability:** Anyone can play without being a programmer. Ready-to-use files provided.

**Buildability:** Source code with clear setup instructions. CMakeLists.txt for cross-platform compilation.

**Scope:** Scaled for 3 members—one polished zone and robust battle system.

**Technology Stack:**
- **Language:** C++17
- **Graphics:** SFML 3.0 (macOS) / SFML 2.6 (Linux)
- **Build System:** CMake 3.10+
- **Testing:** GoogleTest

---

## Software Architecture

### Design Pattern: Layered Architecture

Keeps game visuals (UI) separate from game rules (logic) and save data. This allows independent development of the debugging engine from world exploration.

### Major Components

| Component | Responsibility |
|-----------|-----------------|
| **RenderEngine** | Draws UI and sprites to screen using SFML |
| **Logic Layer** | Decides game state (collisions, battle triggers, scoring) |
| **Debugger** | Text box where players type fixes; validates correctness |
| **Data Manager** | Loads Codémon and level data from JSON files |
| **StateStack** | Manages game states (World, Battle, Menu) |
| **AssetCache** | Manages texture and font memory |

### Data Storage

**Format:** JSON files for easy manual editing  
**Location:** `/assets/data/species.json` and `/assets/data/map_events.json`  
**Schema:**
- `id`: Unique integer
- `name`: String
- `buggy_code`: String containing the error
- `solution_key`: Expected fix substring or logic

### Interfaces (Component Communication)

| Interface | Direction | Content |
|-----------|-----------|---------|
| Logic → Debugger | Request | "Check Code" message |
| Debugger → Logic | Response | "Success" or "Failure" signal |
| Data Manager → Logic | Provides | `std::map<int, SpeciesData>` |
| Logic → RenderEngine | Sends | List of SpriteRequest objects per frame |

---

## Testing Infrastructure

### Unit Testing

**Framework:** GoogleTest (GTest)

**Justification:** Industry standard for C++; integrates natively with CMake; allows mocking for game input/events without opening graphics window.

**How to Add Tests:**
1. Create new `.cpp` file in `/tests` directory
2. Use `TEST(ComponentName, TestName)` macro
3. Register in `tests/CMakeLists.txt`
4. Run with `ctest` command

**Test Coverage:**
- Battle system
- Move validation
- Party management
- Stats calculations
- Player collision detection
- TileMap loading and pathfinding
- State transitions
- Validator functions

### Integration Testing

**Test Scenario: "The First Battle"**

Ensures communication between Core Logic, Debugging Engine, and Visual Layer:

1. **Trigger:** WorldState detects collision with "Buggy Codémon"
2. **Action:** GameStateManager pushes BattleState onto stack
3. **Interaction:** Player selects "Attack" → Logic Layer triggers EditorUI overlay
4. **Validation:** Player input passed to SyntaxValidator
   - **If Pass:** ValidationResult returns true → apply damage
   - **If Fail:** ValidationResult returns false → play error sound, enemy takes turn
5. **Conclusion:** When HP reaches 0, BattleState pops, WorldState resumes

### Usability Testing

**Test Groups:**
- RPG game veterans
- Average computer-proficient users
- Less computer-proficient users

**Metrics:** Time to task completion, hesitation points, post-test interviews

---

## Continuous Integration (CI)

**Service:** GitHub Actions

**Justification:** Natively integrated into GitHub; supports multi-OS; free for students.

**Configuration:** `.github/workflows/cmake-build.yml`

**Triggers:**
- Every push to any branch
- Every pull request to main branch

**Execution:** Full suite of unit and integration tests across Windows, macOS, and Linux

---

## Risk Management

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|-----------|
| Scope creep | 5/10 | 4/10 | Define core loop early; treat advanced features as stretch goals |
| Technical difficulties | 3/10 | 7/10 | Plan features early; simplify implementation if needed |
| Version control issues | 2/10 | 2/10 | Communicate before pushing; maintain backups |
| Schedule slippage | 7/10 | 5/10 | Follow sprints; dedicate weekly hours; team meetings |
| Faulty test cases | 3/10 | 5/10 | Test-driven development; black box + white box testing; rigorous review |

---

## Coding Standards

**C++ Style Guide:** [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

**Key Rules:**
- Smart pointers for memory management
- Const correctness
- Clear naming conventions
- Comprehensive comments

**Enforcement:** Automated formatting with `.clang-format` file in repository

---

## Development Timeline

| Week | Focus | Key Milestones |
|------|-------|-----------------|
| 1 | Requirements & core design | GitHub setup, Milestone 1 |
| 2 | Requirements elicitation | Enhance living doc, Milestone 2 |
| 3 | Begin coding framework | Plan architecture |
| 4 | Define classes & game mechanics | Begin Milestone 3, start test cases |
| 5 | Finalize project plan | Begin mid-term presentation |
| 6 | Executable program, begin UI | Milestone 4, collect external feedback |
| 7 | Finish basic functionality, enhance UI | Milestone 5, write remaining tests |
| 8 | Bug fixing & refinement | Test program, Milestone 6 |
| 9 | Final polish | Final testing, Milestone 7, presentation |

---

## Bug Tracking

**Strategy:** GitHub Issues with labels for severity and category:
- **Severity:** critical, minor
- **Category:** ui, logic, data

**Completion Criteria:** Issue closed by team member other than the author.

---

## Architectural Decisions

### Decision 1: JSON vs. SQLite

**Chosen:** JSON files

**Rationale:** More readable; easier to manually add new challenges; simpler dependency management.

### Decision 2: Centralized vs. Decentralized State Management

**Chosen:** Centralized StateStack

**Rationale:** Prevents spaghetti code; ensures only one state is active; easier to debug.

---

## Future Enhancements

- Advanced visual effects and animations
- Multiplayer challenges
- Custom challenge creator
- Leaderboard system
- Mobile platform support
