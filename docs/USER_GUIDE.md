# Codémon: User Manual

Welcome to **Codémon: A Buggy World**. This guide will help you 
install, run, and play the game.

## 1. High-Level Description
Codémon is a Pokémon-style RPG designed to teach debugging. 
In this world, the environment and the creatures are "buggy." 
Instead of just using brute force, you progress by identifying 
and fixing logic errors in the game’s code snippets.

## 2. Prerequisites & Installation
Codémon supports **macOS**, **Linux**, **Windows**.

### Prerequisites:
* **CMake 3.20+**
* **C++17 compiler** (Clang, GCC, or MSVC)
* **SFML** (3.0 on macOS, 2.6+ on Linux/Windows)

Platform package setup:
- **macOS**: `brew install sfml`
- **Linux (Ubuntu/Debian)**: `sudo apt install libsfml-dev`
- **Windows (vcpkg)**: `vcpkg install sfml:x64-windows`

### Installation:
1. Clone the repository:
   ```bash
   git clone https://github.com/Connor-Allen10/codemon.git
   cd codemon
   ```
2. Configure and build:

   **macOS/Linux**
   ```bash
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build -j
   ```

   **Windows (with vcpkg toolchain)**
   ```powershell
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build build --config Debug
   ```
3. Keep `challenges.txt` with the executable if you want custom runtime challenges.

## 3. How to Run
From project root:

**macOS/Linux**
```bash
./build/codemon
```

**Windows**
```powershell
.\build\Debug\codemon.exe
# or (single-config generators)
.\build\codemon.exe
```

### Optional: Enable Runtime Debug Logs
By default, most state/debug loader logs are quiet to reduce terminal noise.

Enable logs for one run:

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

Available toggles:
- `CODEMON_VERBOSE_STATE_LOGS` → prints state-flow logs (menu start, encounter transitions, battle exit)
- `CODEMON_VERBOSE_CHALLENGE_LOADER` → prints challenge loader parsing/fallback logs

Truthy values: `1`, `true`, `TRUE`, `on`, `ON`

## 4. How to Play
* **Movement:** Use the **Arrow Keys** or **WASD** to walk.
* **Battles:** Battles trigger when you move through grass tiles.
* **Debug Editor:** In battle, press **E** or **F1** to open the debug challenge editor.
* **Debugging:** Type your fix into the editor and press **Submit**.
* A correct fix allows your Codémon to execute its move.
* **Exit Battle:** Press **ESC**.

## 5. How to Report a Bug
If you encounter a crash or a non-gameplay error:
1. Visit our [GitHub Issues Page](https://github.com/Connor-Allen10/codemon/issues).
2. Click **New Issue** and select the **Bug Report** template.
3. Please describe what you were doing and what OS you are using.

## 6. Known Bugs & Limitations
A list of current known issues can be found in our issue tracker 
[here](https://github.com/Connor-Allen10/codemon/issues). 
* Current limitation: The game is optimized for 1920x1080 resolution;
* UI elements may shift on smaller screens.

## 7. FAQ & Troubleshooting

### Build Issues

**Q: CMake says "Could not find SFML"**  
A: Install SFML for your platform:
- macOS: `brew install sfml`
- Windows: `vcpkg install sfml:x64-windows`
- Linux: `sudo apt install libsfml-dev`

**Q: Build fails on Windows with "Cannot find vcpkg"**  
A: Add the CMake toolchain flag:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

**Q: "cmake: command not found"**  
A: Install CMake 3.20+ from [cmake.org](https://cmake.org/download/) or via your package manager.

### Runtime Issues

**Q: Battle text doesn't appear on Windows**  
A: This is a known font loading issue (#14). The battle still works—press ESC to exit. Fix in progress.

**Q: Game crashes immediately on startup**  
A: Verify assets were copied by CMake and run from project root:
```bash
cmake --build build
./build/codemon  # macOS/Linux
```
```powershell
cmake --build build --config Debug
.\build\Debug\codemon.exe  # Windows
```

**Q: Player movement feels laggy or stutters**  
A: Build in Release mode for better performance:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

**Q: Tests fail with "Cannot open map file"**  
A: Build tests, then run from project root or with CTest:
```bash
cmake --build build --target run_tests
./build/run_tests
# or
ctest --test-dir build --output-on-failure
```
```powershell
cmake --build build --config Debug --target run_tests
.\build\Debug\run_tests.exe
# or
ctest --test-dir build -C Debug --output-on-failure
```

**Q: How do I turn debug logs back on?**  
A: Run with one or both env vars enabled:
**macOS/Linux**
```bash
CODEMON_VERBOSE_STATE_LOGS=1 ./build/codemon
CODEMON_VERBOSE_CHALLENGE_LOADER=1 ./build/codemon
```
**Windows PowerShell**
```powershell
$env:CODEMON_VERBOSE_STATE_LOGS='1'; .\build\Debug\codemon.exe
$env:CODEMON_VERBOSE_CHALLENGE_LOADER='1'; .\build\Debug\codemon.exe
```
**Windows CMD**
```bat
set CODEMON_VERBOSE_STATE_LOGS=1 && .\build\Debug\codemon.exe
set CODEMON_VERBOSE_CHALLENGE_LOADER=1 && .\build\Debug\codemon.exe
```

### Gameplay Issues

**Q: I'm stuck in an endless battle loop**  
A: This was fixed in commit `1810356`. Pull the latest changes:
```bash
git pull origin main
```

**Q: How do I trigger battles?**  
A: Walk on grass tiles (green areas). Battles trigger randomly with 15% chance every 0.5 seconds.

**Q: What are the controls?**  
A: 
- Movement: WASD or Arrow Keys
- Debug editor: E or F1 (during battle)
- Exit battle: ESC

**Q: Can I play in fullscreen?**  
A: Currently windowed mode only. Fullscreen support is planned for a future release.

### Still Having Issues?

If your problem isn't listed here:
1. Check [GitHub Issues](https://github.com/Connor-Allen10/codemon/issues) for similar reports
2. Review the [Developer Guide](./DEVELOPER_GUIDE.md) for technical details
3. Open a new issue with:
   - Your OS and version
   - Build logs or error messages
   - Steps to reproduce the problem
