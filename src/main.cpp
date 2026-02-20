#include "game/Game.hpp"

int main() {
    Game game;
    game.run();
    return 0;
}

/*

COMPILE COMMAND

g++ main.cpp battle/Battle.cpp battle/Party.cpp states/BattleState.cpp states/DebugState.cpp states/MainMenuState.cpp states/WorldState.cpp world/Map.cpp world/Obstacle.cpp world/Player.cpp game/Game.cpp game/StateStack.cpp -Iinclude -IC:/SFML/include -LC:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -o bin/codemontest.exe

*/