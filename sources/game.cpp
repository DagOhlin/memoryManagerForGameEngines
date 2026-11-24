#include "game.hpp"

#include "defaultAllocator.hpp"
#include "timer.hpp"

void Game::Run()
{
    // TODO: Tests
    ScopeTimer t("Run timer");
    DefaultAllocator defalloc;
    defalloc.Initialize(0);
    this->FrameAllocatorTestlevel1(defalloc, AllocatorType::Default);
}


void Game::FrameAllocatorTestlevel1(Allocator& allocator, AllocatorType allocatorType){

    const int numberOfAllocations = 10000;
    const int numberOfFrames = 1000;
    void* addressArray[numberOfAllocations];
    ScopeTimer t("FrameAllocatorTestlevel1");


    for (size_t i = 0; i < numberOfFrames; i++)
    {
        for (size_t i = 0; i < numberOfAllocations; i++)
        {
            addressArray[i] = allocator.Allocate(8);
        }
        
        if (allocatorType == AllocatorType::Stack)
        {
            allocator.Clear();
        }
        else{
            for (size_t i = 0; i < numberOfAllocations; i++)
            {
                allocator.Free(addressArray[i]);
            }
        }
    }
    
    
    
    
}
