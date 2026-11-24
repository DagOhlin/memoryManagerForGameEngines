#include "game.hpp"

#include "defaultAllocator.hpp"
#include "timer.hpp"

void Game::Run()
{
    // TODO: Tests
    ScopeTimer t("Run timer");
    DefaultAllocator defalloc;
    defalloc.Initialize(0);
    unsigned long long *stuff = (unsigned long long*)defalloc.Allocate(sizeof(unsigned long long));
    *stuff = 15;
    std::cout << *stuff << std::endl;
    defalloc.Free(stuff);
}
