/**
 * @file main.cpp
 * @brief Entry point for the Codemon game.
 * 
 * Handles executable directory detection and working directory setup
 * to ensure assets can be loaded regardless of where the executable
 * is run from. Then creates and runs the Game instance.
 */

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include "game/Game.hpp"

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#endif

/**
 * @brief Get the directory where the executable is located.
 * @return Path to the directory containing the executable
 * 
 * Platform-specific implementation for Windows, macOS, and Linux.
 * Falls back to current_path() if platform-specific method fails.
 */
std::filesystem::path getExecutableDir()
{
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
#elif defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string path(size, '\0');
    if (_NSGetExecutablePath(path.data(), &size) == 0) {
        return std::filesystem::path(path).parent_path();
    }
    return std::filesystem::current_path();
#elif defined(__linux__)
    char buffer[PATH_MAX];
    const ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (count > 0) {
        buffer[count] = '\0';
        return std::filesystem::path(buffer).parent_path();
    }
    return std::filesystem::current_path();
#else
    return std::filesystem::current_path();
#endif
}

/**
 * @brief Main entry point for the Codemon game.
 * @return 0 on success, non-zero on error
 * 
 * Startup sequence:
 * 1. Detect executable directory
 * 2. Set working directory to project root (one level up from build/)
 * 3. Verify assets/ folder exists
 * 4. Create and run Game instance
 * 5. Catch and report any exceptions
 */
int main()
{
    try
    {
        // ------------------------------------------
        // Fix working directory automatically
        // exe = codemon/src/bin/codemon.exe
        // assets = codemon/assets/
        // so go up TWO folders
        // ------------------------------------------
        std::filesystem::path exeDir = getExecutableDir();

        // Prefer executable directory when build/assets exists.
        // This supports launching with ./build/codemon from repo root.
        const std::filesystem::path buildAssets = exeDir / "assets";
        const std::filesystem::path parentAssets = exeDir.parent_path() / "assets";
        const std::filesystem::path parentSrcAssets = exeDir.parent_path() / "src" / "assets";

        if (std::filesystem::exists(buildAssets)) {
            std::filesystem::current_path(exeDir);
            std::cout << "Assets found - working directory set to: "
                      << std::filesystem::current_path().string() << std::endl;
        } else if (std::filesystem::exists(parentAssets) || std::filesystem::exists(parentSrcAssets)) {
            std::filesystem::current_path(exeDir.parent_path());
            std::cout << "Assets found - working directory set to: "
                      << std::filesystem::current_path().string() << std::endl;
        } else {
            std::cerr << "WARNING: assets/ folder not found in expected location.\n"
                      << "Executable directory: " << exeDir.string() << "\n"
                      << "Current directory: " << std::filesystem::current_path().string() << std::endl;
        }

        Game game;
        game.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
    }

    return 0;
}
/*

COMPILE COMMAND

compile from project root

g++ -std=c++17 `
  src/main.cpp `
  src/battle/Battle.cpp src/battle/Party.cpp `
  src/states/BattleState.cpp src/states/DebugState.cpp src/states/MainMenuState.cpp src/states/WorldState.cpp `
  src/world/TileMap.cpp `
  src/game/Game.cpp src/game/StateStack.cpp `
  -Isrc -Isrc/include `
  -Lsrc/lib `
  -lsfml-graphics -lsfml-window -lsfml-system `
  -o src/bin/codemon.exe

    ===============================
    Run before TGUI compile Command
    ===============================

    1. cd external/TGUI
        mkdir build -Force
        cd build
        cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DTGUI_BACKEND=SFML_GRAPHICS -DSFML_DIR="C:\personal_projects\codemon\src\lib\cmake\SFML"
        mingw32-make

        **note: C:\personal_projects\codemon\src\lib\cmake\SFML    *personal_projects* will need to be adjusted to the folder where you keep your project directory**

    2. go back to project root.

    3. Run compile command below
  

  ============================================
  COMPILE COMMAND (FOR CHUMPS) (INCLUDES TGUI)
  ============================================
  g++ -std=c++17 `
  src/main.cpp `
  src/battle/Battle.cpp src/battle/Party.cpp `
  src/states/BattleState.cpp src/states/DebugState.cpp src/states/MainMenuState.cpp src/states/WorldState.cpp `
  src/world/TileMap.cpp `
  src/game/Game.cpp src/game/StateStack.cpp `
  src/debug/ChallengeLoader.cpp src/debug/DebugChallenge.cpp `
  -Isrc -Isrc/include `
  -Iexternal/TGUI/include `
  -Lsrc/lib `
  -Lexternal/TGUI/build/lib `
  -ltgui `
  -lsfml-graphics -lsfml-window -lsfml-system `
  -o src/bin/codemon.exe
*/


/*test comment*/
