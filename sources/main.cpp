#include <iostream>
#include "game.hpp"

int main(int argc, char *argv[])
{
    std::cout << "Starting..." << std::endl;
    
    Game game;
    game.Run();

    std::cout << "Shutting down..." << std::endl;
}