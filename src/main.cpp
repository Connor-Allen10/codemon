#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include "game/Game.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

// --------------------------------------------------
// Get directory where the executable is located
// --------------------------------------------------
std::filesystem::path getExecutableDir()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
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

        // Move working directory to project root
        std::filesystem::current_path(exeDir.parent_path().parent_path());

        std::cout << "Working directory set to: "
                  << std::filesystem::current_path() << std::endl;

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