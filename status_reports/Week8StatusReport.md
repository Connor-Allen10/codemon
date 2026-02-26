# Weekly Status Report – Week 8 (Milestone 5)

**Project:** Codémon: A Buggy World  
**Team:** Connor Allen, Isaac Hutchison, Lon Danna  
**Date:** February 26, 2026

## 1. Progress Overview
This week, the team focused on transitioning to a professional CI/CD workflow and implementing automated unit testing. While the macOS pipeline is fully operational, the Windows pipeline remains in active debugging due to environment-specific runner errors.

### Tasks Completed:
* **CI/CD Pipeline Integration:** Successfully implemented GitHub Actions for macOS-latest.
* **Automated Testing Infrastructure:** Integrated GoogleTest (GTest) as a project dependency using CMake's `FetchContent`.
* **Asset Pipeline:** Refined the CMake build logic to automatically copy the `/assets` folder to the build directory, ensuring textures load correctly on all platforms.
* **Initial Unit Testing:** Integrated `tests/test_stats.cpp` into the build system to validate core health and stat logic.

## 2. Individual Contributions

### Lon Danna (Software Developer)
* **Accomplishments:** Configured the GitHub Actions workflow. Successfully automated the macOS build and test sequence (36s completion). Integrated GoogleTest into the project structure. Conducted extensive troubleshooting on the Windows MSVC compiler environment regarding test discovery paths.
* **Goals for Next Week:** Resolve the persistent `run_tests_NOT_BUILT` error on the Windows runner by adjusting CTest discovery parameters.

### Connor Allen (Project Manager)
* **Accomplishments:** Worked on SFML sprite implementation and texture management. *(Note: Connor to add details).*
* **Goals for Next Week:** Finalize battle state logic.

### Isaac Hutchison (UI / UX Designer)
* **Accomplishments:** Refined SFML graphical assets and integrated new sprites into the WorldState. *(Note: Isaac to add details).*
* **Goals for Next Week:** Finalize UI scaling.

## 3. Roadblocks & Risks
* **Windows CI Failure:** The Windows runner continues to fail during the test discovery phase (`run_tests_NOT_BUILT`). This is a known issue with how Visual Studio handles multi-configuration builds in virtual environments.
* **Resolution:** Currently testing a shift from `PRE_TEST` to `POST_BUILD` discovery and experimenting with explicit pathing to the `Release/` folder.

## 4. Plans for Next Week
* Fix the Windows CI pipeline.
* Expand unit tests to cover combat math.