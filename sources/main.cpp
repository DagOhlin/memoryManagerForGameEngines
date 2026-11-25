#include <iostream>
#include "game.hpp"
#include "poolAllocator.hpp"

int main(int argc, char *argv[])
{
    std::cout << "Starting..." << std::endl;

    PoolAllocator poolAlloc;
    poolAlloc.Initialize(8, 100);
    int *num = (int *)poolAlloc.Allocate();
    *num = 22;
    std::cout << *num << std::endl;
    poolAlloc.Free(num);
    poolAlloc.Clear();

    Game game;
    game.Run();

    std::cout << "Shutting down..." << std::endl;
}