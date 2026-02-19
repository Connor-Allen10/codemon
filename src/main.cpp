#include "game/Game.hpp"

int main() {
    Game game;
    game.run();
    return 0;
}

/*

COMPILE COMMAND

g++ main.cpp states/WorldState.cpp game/Game.cpp game/StateStack.cpp -I../include -IC:/SFML/include -LC:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -o codemontest.exe

*/