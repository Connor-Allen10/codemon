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

// --------------------------------------------------
// Get directory where the executable is located
// --------------------------------------------------
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

        // Move working directory to project root when running from build/
        // executable path (e.g., <repo>/build/codemon).
        std::filesystem::path candidate = exeDir.parent_path();
        if (std::filesystem::exists(candidate / "assets")) {
            std::filesystem::current_path(candidate);
        }

        std::wcout << L"Working directory set to: "
               << std::filesystem::current_path().wstring() << std::endl;

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
*/