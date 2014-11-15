#include "Game.hpp"
#include <iostream>


using namespace std;

int main(int argc, char** argv){
	Game game;
	game.init();
	game.createWindow();
	game.run();

    return 0;
}
