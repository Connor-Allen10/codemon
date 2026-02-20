# Weekly Status Report – Week 6 (Milestone 4)

**Project:** Codémon: A Buggy World  
**Team:** Connor Allen, Isaac Hutchison, Lon Danna  
**Date:** February 18, 2026

## 1. Progress Overview
This week, the team focused on finalizing Milestone 4 requirements, specifically repository reorganization, user/developer documentation, and standardizing our bug-tracking workflow.

### Tasks Completed:
* **Repository Reorganization:** Created a clean directory structure (`/docs`, `/assets`, `.github/ISSUE_TEMPLATE`) to improve project maintainability.
* **User Documentation:** Drafted a comprehensive `USER_GUIDE.md` covering installation and gameplay mechanics.
* **Developer Documentation:** Created `DEVELOPER_GUIDE.md` with build instructions (CMake), testing protocols, and directory mapping.
* **Bug Tracker Setup:** Implemented a standardized GitHub Issue template for bug reporting and documented known limitations.
* **Core Implementation:** Continued refinement of the Battle and World state transitions.
* **Base SFML Implementation:** Created a prototype of graphical game movement.

## 2. Individual Contributions

### Connor Allen (Project Manager)
* **Accomplishments:** Integrated basic battle functionality and set up the CMake build environment for the team. Added initial unit tests for stats validation.
* **Goals for Next Week:** Finalize the remaining "branching evolution" design logic and oversee final integration.

### Isaac Hutchison (UI / UX Designer)
* **Accomplishments:** Refined the terminal overlay UI for the debugging mechanic. Ensured assets are properly pathed in the new `/assets` folder.
* **Goals for Next Week:** Implement UI animations for battle transitions and menu navigation.

### Lon Danna (Software Developer)
* **Accomplishments:** Led the Milestone 4 documentation effort. Updated the README and repository structure to meet grading criteria.
* **Goals for Next Week:** Complete the pattern-matching engine for the "Bug Fixing" gameplay loop.

## 3. Roadblocks & Risks
* **SFML Dependencies:** Ensuring all team members have the same SFML version linked correctly via CMake. 
* **Resolution:** Documentation added to the Developer Guide to standardize environment setup.

## 4. Plans for Next Week
* Move from Beta implementation to a "Feature Complete" state.
* Conduct internal playtesting to identify and log new bugs in the issue tracker.
