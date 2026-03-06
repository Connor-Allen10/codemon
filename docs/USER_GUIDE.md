# Codémon: User Manual

Welcome to **Codémon: A Buggy World**. This guide will help you 
install, run, and play the game.

## 1. High-Level Description
Codémon is a Pokémon-style RPG designed to teach debugging. 
In this world, the environment and the creatures are "buggy." 
Instead of just using brute force, you progress by identifying 
and fixing logic errors in the game’s code snippets.

## 2. Prerequisites & Installation
Currently, Codémon is supported on **Windows 10/11**.

### Prerequisites:
* **SFML Libraries:** Included in the release folder.
* **C++ Redistributables:** Ensure your system is up to date.

### Installation:
1. Download the latest `Codemon_v0.x.zip` from our GitHub Releases page.
2. Extract the folder to your desired location.
3. **Note:** Do not move the `Codemon.exe` out of the folder,
4. as it needs the `assets/` and `bin/` folders to run.

## 3. How to Run
1. Navigate to the extracted folder.
2. Double-click **Codemon.exe**.
3. If prompted by Windows Defender, select "More Info" -> "Run Anyway."

## 4. How to Play
* **Movement:** Use the **Arrow Keys** or **WASD** to walk.
* **Interaction:** Press **E** or **Space** to interact with obstacles
* (like broken bridges).
* **Battles:** When a battle begins, use the **Terminal Overlay**
* to inspect the Codémon's code. 
* **Debugging:** Type your fix into the editor and press **Submit**.
* A correct fix allows your Codémon to execute its move!

> **Work in Progress:** The "Evolution" system and "Branching Evolution"
> are currently under development and will be included in the final version.

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
A: Install CMake 3.10+ from [cmake.org](https://cmake.org/download/) or via your package manager.

### Runtime Issues

**Q: Battle text doesn't appear on Windows**  
A: This is a known font loading issue (#14). The battle still works—press ESC to exit. Fix in progress.

**Q: Game crashes immediately on startup**  
A: Verify you're running from the correct directory with `assets/` folder present:
```bash
cd build
./codemon  # macOS/Linux
```

**Q: Player movement feels laggy or stutters**  
A: Build in Release mode for better performance:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

**Q: Tests fail with "Cannot open map file"**  
A: Ensure you're running tests from the build directory:
```bash
cd build
ctest --output-on-failure
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
- Debug overlay: F1
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
