# Codémon: A Buggy World 👾

Codémon is a Pokémon-style RPG built in C++ using SFML. In this world, the core gameplay mechanic is **debugging**. Players progress by identifying and fixing logic errors in the code of creatures and the environment.

---

## ⚡ Quick Start

### Prerequisites
- **CMake 3.10 or higher** (check version: `cmake --version`)
- **C++ compiler** with C++17 support (GCC 7+, Clang 5+, MSVC 2017+)
- **SFML library:**
  - macOS: `brew install sfml` (SFML 3.0)
  - Windows: `vcpkg install sfml:x64-windows` (SFML 2.6)
  - Linux: `sudo apt install libsfml-dev` (Ubuntu/Debian)

### Build Instructions

**Platform-Specific Notes:**
- **macOS:** Uses SFML 3.0. Build should work out of the box with installed SFML.
- **Windows:** Install SFML via `vcpkg install sfml:x64-windows` and use `-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake`
- **Linux:** Ubuntu/Debian users should install `libsfml-dev` package.

```bash
# Clone the repository
git clone https://github.com/Connor-Allen10/codemon.git
cd codemon

# Create and configure build
mkdir -p build && cd build
cmake ..
cmake --build . -j

# Or for Release mode (faster build and runtime)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Test Instructions
```bash
# From project root
cd build
ctest --output-on-failure
```
**Expected:** 31 tests passing (25 core logic + 6 state transition tests).

### Run Instructions
```bash
# From build directory
./codemon           # macOS/Linux
./Release/codemon.exe  # Windows (Release mode)
```

**Controls:**
- **Movement:** WASD or Arrow Keys
- **Debug Overlay:** F1 (toggle)
- **Exit Battle:** ESC

---

## 🎮 Operational Use Cases

### ✅ World → Battle Transition (Beta Release)
**Status:** Fully implemented and tested
1. Start the game - player spawns in tilemap world
2. Walk on grass tiles (darker green areas)
3. After ~0.5 seconds on grass, battle triggers randomly (15% chance per tick)
4. Purple battle screen appears with "Battle!" text
5. Press **ESC** to exit battle and return to world
6. Walk around world freely with animated movement

**Components Involved:**
- World rendering (TileMap, Camera)
- Player movement (collision detection, animation)
- State machine (push/pop mechanics)
- Battle initialization
- Encounter triggering logic

---

## 📖 Full Documentation
For detailed information, see:

* **[User Manual](./docs/USER_GUIDE.md)**: How to install, run, and play the game.
* **[Developer Guidelines](./docs/DEVELOPER_GUIDE.md)**: Architecture, directory structure, and build system details.
* **[Living Document](./docs/LIVING_DOCUMENT.md)**: Complete design overview, use cases, architecture, and technical decisions.
* **[Test Plan & CI Infrastructure](./docs/TEST_PLAN.md)**: Testing strategy and GitHub Actions CI setup.

---

## 🛠 Project Status (Milestone 6: Beta Release)
We are entering the beta testing phase with core components operational.
* **Core Logic:** World rendering, player movement, random encounters, and battle state transitions fully implemented.
* **Testing:** 31 automated tests passing with GitHub Actions CI integration.
* **Quality:** Comprehensive code documentation (Doxygen-style) and cross-platform compatibility (macOS/Windows/Linux).
* **Next Phase:** Battle UI prototype implementation and sprite asset integration.

## 👥 The Team
* **Connor Allen:** Project Manager
* **Isaac Hutchison:** UI / UX Designer
* **Lon Danna:** Software Developer & Test Engineer

---

## 🐞 Bug Reporting
If you find a bug, please help us by reporting it:
1. Go to the **[Issues](../../issues)** tab.
2. Click **New Issue**.
3. Use the **[Bug Report Template](../../issues/new?template=bug_report.md)** to provide the necessary details (OS, steps to reproduce, etc.).

*Feedback from beta testing is documented in the [beta-testing](./beta-testing/) folder.*
