# Codémon: Developer Guidelines

This document provides instructions for setting up the development 
environment, understanding the architecture, and contributing to 
the Codémon project.

## 1. Obtaining the Source Code
Clone the repository using Git:
```bash
git clone https://github.com/Connor-Allen10/codemon.git
```

## 2. Directory Structure
* `/src`: Contains all .cpp and .hpp source files (Game logic, UI, Engine).
* `/src/assets`: Textures, fonts, and map data used by the game.
* `/challenges.txt`: External debug challenge definitions loaded at runtime (with fallback defaults if missing/invalid).
* `/docs`: Project documentation, user manuals, and milestone reports.
* `/external`: Vendored third-party dependencies (for example, TGUI source and GoogleTest via CMake FetchContent cache/build outputs).
* `/status_reports`: Weekly progress updates and sprint planning.

## 3. How to Build
We use CMake as our build system to manage dependencies like SFML.

### Prerequisites:
* C++ Compiler: GCC, Clang, or MSVC (supporting C++17 or higher).
* CMake: Version 3.20 or higher.
* SFML: Version 3.0 or 2.6+ (project auto-detects and links either).
* TGUI: Optional (enabled when available; project still builds without it).

### Build Steps:
1. Open a terminal in the project root.
2. Configure CMake: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
3. Compile: `cmake --build build -j`

Platform notes:
- macOS: Homebrew SFML is detected via `/opt/homebrew`.
- Linux: System SFML packages under `/usr` or `/usr/local` are detected.
- Windows: Use vcpkg/toolchain setup when needed.

## 4. Testing the Software
We use an automated unit testing suite with GoogleTest to verify game logic.

To run tests locally, execute:
```bash
cmake --build build --target run_tests
./build/run_tests
```

Or run through CTest discovery:
```bash
ctest --test-dir build --output-on-failure
```

For detailed information on testing infrastructure and continuous integration, see [TEST_PLAN.md](TEST_PLAN.md).

### Debug Environment Variables
To keep normal output clean, several diagnostics are runtime-gated.

- `CODEMON_VERBOSE_STATE_LOGS`
	- Enables state transition/action logs from world/menu/battle flow.
- `CODEMON_VERBOSE_CHALLENGE_LOADER`
	- Enables challenge parser/fallback diagnostics.

Truthy values accepted by both: `1`, `true`, `TRUE`, `on`, `ON`

Examples (single run):

**macOS/Linux (bash/zsh)**
```bash
CODEMON_VERBOSE_STATE_LOGS=1 CODEMON_VERBOSE_CHALLENGE_LOADER=1 ./build/codemon
```

**Windows (PowerShell)**
```powershell
$env:CODEMON_VERBOSE_STATE_LOGS='1'; $env:CODEMON_VERBOSE_CHALLENGE_LOADER='1'; .\build\Debug\codemon.exe
```

**Windows (CMD)**
```bat
set CODEMON_VERBOSE_STATE_LOGS=1 && set CODEMON_VERBOSE_CHALLENGE_LOADER=1 && .\build\Debug\codemon.exe
```

## 5. Building a Release
To package a release for end-users:
1. Configure in Release mode: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`
2. Build the project: `cmake --build build -j`
3. Create local distribution folder: `cmake --build build --target package_dist`
4. Distribution output is generated at `/dist/codemon` with:
	- executable (`codemon` or `codemon.exe`)
	- `assets/`
	- `challenges.txt`
	- `codemonNames.txt`
5. Ensure all necessary SFML runtime binaries (.dll/.dylib/.so) are included for external distribution.

## 6. Coding Guidelines
* Style: We follow the Google C++ Style Guide for consistency.
* Memory: Use Smart Pointers (std::unique_ptr, std::shared_ptr) to avoid memory leaks.
* Workflow: All feature contributions must be made via feature branches. Each Pull Request must be reviewed before merging to main.
