# Weekly Status Report – Week 8 (Milestone 5)

**Project:** Codémon: A Buggy World  
**Team:** Connor Allen, Isaac Hutchison, Lon Danna  
**Date:** February 26, 2026

## 1. Progress Overview
This week, the team implemented a CI/CD pipeline and integrated GoogleTest. While the macOS build is fully automated and passing, the Windows pipeline is currently experiencing a known "discovery" issue with Visual Studio's multi-config directory structure.

### Tasks Completed:
* **CI/CD Integration:** Configured GitHub Actions for Mac and Windows. macOS builds and tests successfully in under 40 seconds.
* **Testing Infrastructure:** Integrated GoogleTest via CMake `FetchContent`.
* **Cross-Platform Fixes:** Resolved SFML linking issues on Windows using `vcpkg`.
* **Asset Management:** Automated asset copying to ensure textures are available at runtime.

## 2. Individual Contributions

### Lon Danna (Software Developer)
* **Accomplishments:** Led the transition to automated CI/CD. Resolved SFML dependency blocks on Windows runners. Developed the initial `test_stats.cpp` suite to validate character health logic. 
* **Goals for Next Week:** Debug the Windows `run_tests_NOT_BUILT` error by refining the CTest discovery path.

### Connor Allen (Project Manager)
* **Accomplishments:** Worked on SFML sprite implementation and texture management, tested beta distribution and execution, .
* **Goals for Next Week:** Finalize battle state logic, add main menu state, add help menu.

### Isaac Hutchison (UI / UX Designer)
* **Accomplishments:** Refined SFML graphical assets and integrated new sprites. Integrated sprites with the map.
* **Goals for Next Week:** Finalize UI scaling for different resolutions.

## 3. Roadblocks & Risks
* **Windows CI Build/Test Conflict:** The Windows runner was failing because the MSVC compiler attempted to execute tests before the build artifacts were fully localized in the `Release/` folder.
* **Resolution:** Reconfigured the GitHub Actions workflow to use explicit target building (`--target codemon --target run_tests`). This bypasses the default MSVC "post-build" test trigger, allowing the Build and Test phases to run as truly independent steps.

## 4. Plans for Next Week
* Stabilize Windows CI.
* Expand unit test coverage for the combat math engine.
