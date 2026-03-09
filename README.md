# Codémon: A Buggy World 👾

Codémon is a Pokémon-style RPG built in C++ using SFML. In this world, the core gameplay mechanic is **debugging**. Players progress by identifying and fixing logic errors in the code of creatures and the environment.

---

## ⚡ Quick Start

### Prerequisites
- **CMake 3.20 or higher** (check version: `cmake --version`)
- **C++ compiler** with C++17 support (GCC 7+, Clang 5+, MSVC 2017+)
- **SFML library:**
  - macOS: `brew install sfml` (SFML 3.0)
  - Windows: `vcpkg install sfml:x64-windows` (SFML 2.6)
  - Linux: `sudo apt install libsfml-dev` (Ubuntu/Debian)
- **TGUI (optional, macOS only):** Bundled in `external/TGUI/` - builds from source automatically
  - Windows: Pre-built DLLs in `external/TGUI/bin/` (tgui.dll, tgui-d.dll)
  - Enables debug code viewer popup (E or F1 key)

### Build Instructions

**Platform-Specific Notes:**
- **macOS:** Uses SFML 3.0 + TGUI (built from source). Build is fully automatic with CMake.
- **Windows:** Install SFML via `vcpkg install sfml:x64-windows` and use `-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake`. TGUI DLLs included.
- **Linux:** Ubuntu/Debian users should install `libsfml-dev` package. TGUI not available on Linux CI.

```bash
# Clone the repository
git clone https://github.com/Connor-Allen10/codemon.git
cd codemon

# Create and configure build (CMake auto-detects platform)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

### Test Instructions
```bash
# From project root (macOS)
cmake --build build --target run_tests
./build/run_tests
```
Or with CTest:
```bash
ctest --test-dir build --output-on-failure
```
**Expected:** 43 tests passing (37 core logic + 6 ChallengeLoader tests).

### Run Instructions
```bash
# From project root (macOS/Linux)
./build/codemon
```

```powershell
# From project root (Windows)
.\build\Debug\codemon.exe
```

**Gameplay Features (macOS/Windows):**
- **Movement:** WASD or Arrow Keys
- **Debug Challenges:** E or F1 - opens debug challenge editor during battles
  - Read the challenge prompt (shows broken code)
  - Type the corrected code in the text area
  - Click Submit to validate your fix
  - Required keyword hint displayed at bottom
  - Cancel button exits without submitting
- **Challenge Customization:** Edit `challenges.txt` to add your own debug puzzles
  - Format: `prompt|solution|keyword` (one per line)
  - Falls back to built-in challenges if file missing
- **Exit Battle:** ESC
- **Controls:** See [User Manual](./docs/USER_GUIDE.md) for extended controls

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
* **Core Logic:** World rendering, player movement, random encounters, battle state transitions, and debug challenge system fully implemented.
* **Testing:** 43 automated tests passing with GitHub Actions CI integration.
* **Quality:** Comprehensive code documentation (Doxygen-style) and cross-platform compatibility (macOS/Linux, Windows CI disabled).
* **Next Phase:** Battle UI enhancements, sprite asset integration, and additional challenge content.

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
