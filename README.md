# Codémon: A Buggy World 👾

Codémon is a Pokémon-style RPG built in C++ using SFML. In this world, the core gameplay mechanic is **debugging**. Players progress by identifying and fixing logic errors in the code of creatures and the environment.

---


## ⚡ Quick Start
### Download and Launch
Download the latest release of codemon and find the `codemon` executable in the `build/` folder (or `build/Debug/codemon.exe` on Windows). 

Codemon is open source! To build it yourself, follow the instructions below.
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
**Expected:** 56 tests passing (unit + integration).

Optional verbose runtime/test logging (cross-platform shell examples) is documented in [Developer Guidelines](./docs/DEVELOPER_GUIDE.md).

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
  - Preferred format: multiline challenge blocks (`BEGIN_CHALLENGE` / `PROMPT` / `SOLUTION` / `KEYWORD` / `END_CHALLENGE`)
  - Legacy format still supported: `prompt|solution|keyword` (one per line)
  - Falls back to built-in challenges if file missing
- **Exit Battle:** ESC
- **Controls:** See [User Manual](./docs/USER_GUIDE.md) for extended controls

---

## 🎮 Operational Use Cases

### ✅ World → Battle Transition
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
* **[INSTALL.md](./INSTALL.md)**: End-user installation guide — platform prerequisites, step-by-step install, gameplay controls, and troubleshooting. Assumes no prior knowledge of the toolset.
* **[SETUP.md](./SETUP.md)**: System administrator / developer deployment guide — all libraries required, build commands, asset packaging, and distribution instructions.
* **[Developer Guidelines](./docs/DEVELOPER_GUIDE.md)**: Coding standards (Google C++ Style Guide), architecture, directory structure, and contribution workflow.
* **[Test Plan & CI Infrastructure](./docs/TEST_PLAN.md)**: How to write, document, and run tests; unit and integration test coverage; GitHub Actions CI setup.
* **[Living Document](./docs/LIVING_DOCUMENT.md)**: Full design overview, use cases, architecture decisions, and team Reflections.

---

## � Release Notes — Version 1.0 (March 2026)

> **Source repository:** https://github.com/Connor-Allen10/codemon  
> **Change log:** `git log --oneline` in the repository root  
> **Issue tracker:** [GitHub Issues](https://github.com/Connor-Allen10/codemon/issues)

### ✅ Working Features
| Feature | Status |
|---------|--------|
| Tile-based overworld with WASD / Arrow Key movement | ✅ |
| Collision detection and camera follow | ✅ |
| Random encounters on grass tiles (15% chance per tick) | ✅ |
| Battle flow: encounter intro → party selection → debug battle | ✅ |
| In-battle debug challenge editor (E or F1) | ✅ |
| Multiline challenge prompts, keyword hints, Submit / Cancel | ✅ |
| External challenge loading from `challenges.txt` with fallback | ✅ |
| Monster sprites, updated main menu visuals | ✅ |
| ESC exits battle and returns to world | ✅ |
| Cross-platform build: macOS (SFML 3.x) + Linux CI (SFML 2.6) | ✅ |

### 🐞 Known Issues
| Issue | Platform | Severity | Issue # |
|-------|----------|----------|---------|
| Battle text may not render correctly (font path) | Windows | Minor | [#14](https://github.com/Connor-Allen10/codemon/issues/14) |
| TGUI debug popup unavailable on Linux CI | Linux | Minor | — |
| UI elements may shift on resolutions below 1920×1080 | All | Minor | — |

All known bugs are tracked in [GitHub Issues](https://github.com/Connor-Allen10/codemon/issues) with `critical`/`minor` severity and `ui`/`logic`/`data` category labels. Issues are closed by a team member other than the author.

### 🧪 Test Results
**56 / 56 tests passing** — GoogleTest unit + integration suite

| Test Suite | Tests | Coverage |
|-----------|-------|----------|
| `BattleStateTest` | 13 | Battle phases, input handling, rendering |
| `ChallengeLoaderTest` | 7 | File parsing, fallback, malformed input, verbose-toggle behavior |
| `DebugIntegrationTest` | 3 | Challenge → battle integration |
| `DebugChallengeTest` | 4 | Submission validation, hints, victory |
| `DebugEngineTest` | 4 | Engine lifecycle, submit pass/fail |
| `StateTransitionTest` | 6 | Push/pop state machine |
| `StatsTest` | 2 | Health reduction, floor validation |
| `TileMapTest` | 4 | CSV loading, passability, bounds |
| `ValidatorsTest` + `ValidatorsEdgeTest` | 9 | Input sanitization, keyword matching |
| `VerticalSliceTest` | 3 | End-to-end state stack smoke tests |

**Run all tests (from project root):**
```bash
ctest --test-dir build --output-on-failure
```
**Run unit tests directly:**
```bash
./build/run_tests
```
See [docs/TEST_PLAN.md](./docs/TEST_PLAN.md) for instructions on adding new tests and CI details.

### 📦 Source Distribution
- **Coding guidelines:** [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) — see [docs/DEVELOPER_GUIDE.md](./docs/DEVELOPER_GUIDE.md)
- **All features** developed on feature branches with PR review before merging to `main`
- **Change log:** `git log --oneline` or `git log --follow -p <file>` to trace evolution of any file
- **Issue tracking:** [GitHub Issues](https://github.com/Connor-Allen10/codemon/issues) — used for bugs, tasks, and cross-developer handoff
- **Inline documentation:** Doxygen-style comments throughout `src/`
- **Build from source:** See [SETUP.md](./SETUP.md) for complete administrator / developer build steps

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
