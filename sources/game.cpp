#include "game.hpp"

#include "defaultAllocator.hpp"
#include "poolAllocator.hpp"
#include "timer.hpp"

void Game::Run()
{
    // TODO: Tests
    ScopeTimer t("Run timer");
    DefaultAllocator defalloc;
    PoolAllocator poolAloc;
    poolAloc.Initialize(16, 15000);
    defalloc.Initialize(0);
    this->FrameAllocatorTestlevel1(defalloc, AllocatorType::Default);
    PoolAllocator poolAloc2;
    poolAloc2.Initialize(256, 30000);
    this->FrameAllocatorTestlevel2(defalloc, AllocatorType::Default);
}


void Game::FrameAllocatorTestlevel1(Allocator& allocator, AllocatorType allocatorType){

    double allocTotalMs = 0.0;
    double freeTotalMs = 0.0;

    const int numberOfAllocations = 10000;
    const int numberOfFrames = 1000;
    const int allocationSize = 8;
    void* addressArray[numberOfAllocations];
    ScopeTimer t("FrameAllocatorTestlevel1");


    for (size_t i = 0; i < numberOfFrames; i++)
    {
        double ms = 0.0;
        {
            ScopeTimer t(ms);
            for (size_t i = 0; i < numberOfAllocations; i++)
                addressArray[i] = allocator.Allocate(allocationSize);
        }
        allocTotalMs += ms;
        {
            ScopeTimer t2(ms);
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
        freeTotalMs += ms;
    }
    std::cout << "alloc per frame: "   << allocTotalMs / numberOfFrames   << " ms\n";
    std::cout << "release per frame: " << freeTotalMs / numberOfFrames << " ms\n";
}

void Game::FrameAllocatorTestlevel2(Allocator& allocator, AllocatorType allocatorType){

    double allocTotalMs = 0.0;
    double freeTotalMs = 0.0;

    const int maxAllocations = 20000;
    const int minAllocations = 1000;
    const int numberOfFrames = 1000;
    const unsigned int minSize = 8;
    const unsigned int maxSize = 256;
    void* addressArray[maxAllocations];
    unsigned int sizes[maxAllocations];

    std::mt19937 rng(1337);
    std::uniform_int_distribution<int> countDist(minAllocations, maxAllocations);
    std::uniform_int_distribution<unsigned int> sizeDist(minSize, maxSize);

    ScopeTimer t("FrameAllocatorTestlevel2");

    for (size_t frame = 0; frame < numberOfFrames; frame++)
    {
        const int numberOfAllocations = countDist(rng);
        for (int i = 0; i < numberOfAllocations; i++)
            sizes[i] = sizeDist(rng);

        double ms = 0.0;
        {
            ScopeTimer t1(ms);
            for (int i = 0; i < numberOfAllocations; i++)
                addressArray[i] = allocator.Allocate(sizes[i]);
        }
        allocTotalMs += ms;
        {
            ScopeTimer t2(ms);
            if (allocatorType == AllocatorType::Stack)
            {
                allocator.Clear();
            }
            else{
                for (int i = 0; i < numberOfAllocations; i++)
                {
                    allocator.Free(addressArray[i]);
                }
            }
        }
        freeTotalMs += ms;
    }
    std::cout << "alloc per frame: "   << allocTotalMs / numberOfFrames   << " ms\n";
    std::cout << "release per frame: " << freeTotalMs / numberOfFrames << " ms\n";
}