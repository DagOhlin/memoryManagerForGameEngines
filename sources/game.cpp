#include "game.hpp"

#include "defaultAllocator.hpp"

void Game::Run()
{
    // TODO: Tests

    DefaultAllocator defalloc;
    defalloc.Initialize(0);
    unsigned long long *stuff = (unsigned long long*)defalloc.Allocate(sizeof(unsigned long long));
    *stuff = 15;
    std::cout << *stuff << std::endl;
    defalloc.Free(stuff);
}
