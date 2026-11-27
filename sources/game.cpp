#include "game.hpp"

#include "defaultAllocator.hpp"
#include "poolAllocator.hpp"
#include "stackAllocator.hpp"
#include "timer.hpp"
#include "buddyAllocator.hpp"

void Game::Run()
{
    // TODO: Tests
    DefaultAllocator def;

    /*
    ScopeTimer t("Run timer");
    DefaultAllocator defalloc;
    PoolAllocator poolAloc;
    poolAloc.Initialize(16, 15000);
    defalloc.Initialize(0);
    this->FrameAllocatorTestlevel1(poolAloc, AllocatorType::Pool);
    PoolAllocator poolAloc2;
    poolAloc2.Initialize(256, 30000);
    this->FrameAllocatorTestlevel2(poolAloc2, AllocatorType::Pool);
    */
    /*PoolAllocator poolForL1;
    poolForL1.Initialize(32, 10000);
    PoolAllocatorTestlevel1(poolForL1, AllocatorType::Pool);

    DefaultAllocator def;
    PoolAllocatorTestlevel1(def, AllocatorType::Default);

    PoolAllocator poolForL2;
    poolForL2.Initialize(32, 10000);
    PoolAllocatorTestlevel2(poolForL2, AllocatorType::Pool);

    PoolAllocatorTestlevel2(def, AllocatorType::Default);


    DefaultAllocator def;
    StackAllocator stackForL1;
    stackForL1.Initialize(1024*1024);
    FrameAllocatorTestlevel1(stackForL1, AllocatorType::Stack);
    FrameAllocatorTestlevel1(def, AllocatorType::Default);

    StackAllocator stackforL2;
    stackforL2.Initialize(1024*1024*8);
    FrameAllocatorTestlevel2(stackforL2, AllocatorType::Stack);
    FrameAllocatorTestlevel2(def, AllocatorType::Default);





    BuddyAllocator buddy;
    buddy.Initialize(32 * 1024 * 1024);
    BuddyAllocatorTestlevel2(buddy, AllocatorType::Buddy);

    
    BuddyAllocatorTestlevel2(def, AllocatorType::Default);
    */

    /*
    DefaultAllocator def;
    def.Initialize(0);
    ThreadAllocatorTest(def, AllocatorType::Default);

    PoolAllocator pool;
    pool.Initialize(32, 80000);
    ThreadAllocatorTest(pool, AllocatorType::Pool);

    BuddyAllocator buddy;
    buddy.Initialize(8 * 1024 * 1024);
    ThreadAllocatorTest(buddy, AllocatorType::Buddy);

    StackAllocator stack;
    stack.Initialize(8 * 1024 * 1024);
    ThreadAllocatorTest(stack, AllocatorType::Stack);
    */

    for (int count : {1000, 10000, 100000})
    {
        PoolAllocator pool;
        pool.Initialize(32, count);
        PoolAllocatorTestlevel1(pool, AllocatorType::Pool, count, 32);
        PoolAllocatorTestlevel1(def, AllocatorType::Default, count, 32);
    }
}


void Game::FrameAllocatorTestlevel1(Allocator& allocator, AllocatorType allocatorType,
                              int numberOfAllocations, unsigned int allocationSize){

    double allocTotalMs = 0.0;
    double freeTotalMs = 0.0;

    //const int numberOfAllocations = 10000;
    const int numberOfFrames = 1000;
    //const int allocationSize = 8;
    void* addressArray[numberOfAllocations];
    ScopeTimer t(std::string("FrameAllocatorTestlevel1 ") + AllocatorName(allocatorType));


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
    std::cout << AllocatorName(allocatorType) << " alloc per frame: "   << allocTotalMs / numberOfFrames << " ms\n";
    std::cout << AllocatorName(allocatorType) << " release per frame: " << freeTotalMs / numberOfFrames  << " ms\n";
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

    ScopeTimer t(std::string("FrameAllocatorTestlevel2 ") + AllocatorName(allocatorType));

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
    std::cout << AllocatorName(allocatorType) << " alloc per frame: "   << allocTotalMs / numberOfFrames << " ms\n";
    std::cout << AllocatorName(allocatorType) << " release per frame: " << freeTotalMs / numberOfFrames  << " ms\n";
}

void Game::PoolAllocatorTestlevel1(Allocator& allocator, AllocatorType allocatorType,
                              int numberOfAllocations, unsigned int allocationSize){

    double allocTotalMs = 0.0;
    double freeTotalMs = 0.0;

    //const int numberOfAllocations = 10000;
    const int numberOfRounds = 1000;
    //const int allocationSize = 32;
    void* addressArray[numberOfAllocations];
    int freeOrder[numberOfAllocations];

    for (int i = 0; i < numberOfAllocations; i++)
        freeOrder[i] = i;

    std::mt19937 rng(1337);
    std::shuffle(freeOrder, freeOrder + numberOfAllocations, rng);

    ScopeTimer t(std::string("PoolAllocatorTestlevel1 ") + AllocatorName(allocatorType));

    for (int round = 0; round < numberOfRounds; round++)
    {
        double ms = 0.0;
        {
            ScopeTimer t1(ms);
            for (int i = 0; i < numberOfAllocations; i++)
                addressArray[i] = allocator.Allocate(allocationSize);
        }
        allocTotalMs += ms;
        {
            ScopeTimer t2(ms);
            for (int i = 0; i < numberOfAllocations; i++)
                allocator.Free(addressArray[freeOrder[i]]);
        }
        freeTotalMs += ms;
    }
    std::cout << AllocatorName(allocatorType) << " alloc per round: " << allocTotalMs / numberOfRounds << " ms\n";
    std::cout << AllocatorName(allocatorType) << " free per round: "  << freeTotalMs / numberOfRounds  << " ms\n";
}

void Game::PoolAllocatorTestlevel2(Allocator& allocator, AllocatorType allocatorType){

    double allocTotalMs = 0.0;
    double freeTotalMs = 0.0;

    const int numberOfObjects = 10000;
    const int replacedPerFrame = 1000;
    const int numberOfFrames = 1000;
    const int allocationSize = 32;
    void* addressArray[numberOfObjects];
    int indices[numberOfObjects];

    for (int i = 0; i < numberOfObjects; i++)
        indices[i] = i;

    std::mt19937 rng(1337);

    for (int i = 0; i < numberOfObjects; i++)
        addressArray[i] = allocator.Allocate(allocationSize);

    ScopeTimer t(std::string("PoolAllocatorTestlevel2 ") + AllocatorName(allocatorType));

    for (int frame = 0; frame < numberOfFrames; frame++)
    {
        std::shuffle(indices, indices + numberOfObjects, rng);

        double ms = 0.0;
        {
            ScopeTimer t1(ms);
            for (int i = 0; i < replacedPerFrame; i++)
                allocator.Free(addressArray[indices[i]]);
        }
        freeTotalMs += ms;
        {
            ScopeTimer t2(ms);
            for (int i = 0; i < replacedPerFrame; i++)
                addressArray[indices[i]] = allocator.Allocate(allocationSize);
        }
        allocTotalMs += ms;
    }

    for (int i = 0; i < numberOfObjects; i++)
        allocator.Free(addressArray[i]);

    std::cout << AllocatorName(allocatorType) << " alloc per frame: " << allocTotalMs / numberOfFrames << " ms\n";
    std::cout << AllocatorName(allocatorType) << " free per frame: "  << freeTotalMs / numberOfFrames  << " ms\n";
}

void Game::BuddyAllocatorTestlevel2(Allocator& allocator, AllocatorType allocatorType){

    double allocTotalMs = 0.0;
    double freeTotalMs = 0.0;

    const int numberOfObjects = 10000;
    const int replacedPerFrame = 1000;
    const int numberOfFrames = 1000;
    const unsigned int minSize = 16;
    const unsigned int maxSize = 1024;
    void* addressArray[numberOfObjects];
    int indices[numberOfObjects];
    unsigned int sizes[replacedPerFrame];
    int failedAllocations = 0;

    for (int i = 0; i < numberOfObjects; i++)
        indices[i] = i;

    std::mt19937 rng(1337);
    std::uniform_int_distribution<unsigned int> sizeDist(minSize, maxSize);

    for (int i = 0; i < numberOfObjects; i++)
        addressArray[i] = allocator.Allocate(sizeDist(rng));

    ScopeTimer t(std::string("BuddyAllocatorTestlevel2 ") + AllocatorName(allocatorType));

    for (int frame = 0; frame < numberOfFrames; frame++)
    {
        std::shuffle(indices, indices + numberOfObjects, rng);
        for (int i = 0; i < replacedPerFrame; i++)
            sizes[i] = sizeDist(rng);

        double ms = 0.0;
        {
            ScopeTimer t1(ms);
            for (int i = 0; i < replacedPerFrame; i++)
                if (addressArray[indices[i]] != nullptr)
                    allocator.Free(addressArray[indices[i]]);
        }
        freeTotalMs += ms;
        {
            ScopeTimer t2(ms);
            for (int i = 0; i < replacedPerFrame; i++)
                addressArray[indices[i]] = allocator.Allocate(sizes[i]);
        }
        allocTotalMs += ms;

        for (int i = 0; i < replacedPerFrame; i++)
            if (addressArray[indices[i]] == nullptr)
                failedAllocations++;
    }

    for (int i = 0; i < numberOfObjects; i++)
        if (addressArray[i] != nullptr)
            allocator.Free(addressArray[i]);

    std::cout << AllocatorName(allocatorType) << " alloc per frame: " << allocTotalMs / numberOfFrames << " ms\n";
    std::cout << AllocatorName(allocatorType) << " free per frame: "  << freeTotalMs / numberOfFrames  << " ms\n";
    if (failedAllocations > 0)
        std::cout << AllocatorName(allocatorType) << " failed allocations: " << failedAllocations << " (results not valid)\n";
}

static void ThreadWork(Allocator* allocator, std::vector<void*>* blocks, int threadId,
                       int allocationsPerThread, unsigned int allocationSize, bool freeAndReallocate)
{
    const unsigned char pattern = (unsigned char)(threadId + 1);

    for (int i = 0; i < allocationsPerThread; i++)
    {
        unsigned char* block = (unsigned char*)allocator->Allocate(allocationSize);
        if (block == nullptr)
            continue;
        for (unsigned int b = 0; b < allocationSize; b++)
            block[b] = pattern;
        blocks->push_back(block);
    }

    if (freeAndReallocate)
    {
        for (size_t i = 0; i < blocks->size(); i += 2)
        {
            allocator->Free((*blocks)[i]);
            unsigned char* block = (unsigned char*)allocator->Allocate(allocationSize);
            if (block != nullptr)
                for (unsigned int b = 0; b < allocationSize; b++)
                    block[b] = pattern;
            (*blocks)[i] = block;
        }
    }
}

void Game::ThreadAllocatorTest(Allocator& allocator, AllocatorType allocatorType){

    const int numberOfThreads = 8;
    const int allocationsPerThread = 10000;
    const unsigned int allocationSize = 32;

    std::vector<std::vector<void*>> threadBlocks(numberOfThreads);
    std::vector<std::thread> threads;

    {
        //ScopeTimer t(std::string("ThreadAllocatorTest ") + AllocatorName(allocatorType));

        
        for (int threadId = 0; threadId < numberOfThreads; threadId++)
        {
            threads.emplace_back(ThreadWork, &allocator, &threadBlocks[threadId], threadId,
                allocationsPerThread, allocationSize, allocatorType != AllocatorType::Stack);
        }
        

        for (std::thread& thread : threads)
            thread.join();
    }

    int failedAllocations = 0;
    int corruptedBlocks = 0;
    int overlappingBlocks = 0;
    std::vector<std::uintptr_t> addresses;

    for (int threadId = 0; threadId < numberOfThreads; threadId++)
    {
        const unsigned char pattern = (unsigned char)(threadId + 1);
        for (void* pointer : threadBlocks[threadId])
        {
            if (pointer == nullptr)
            {
                failedAllocations++;
                continue;
            }
            unsigned char* block = (unsigned char*)pointer;
            for (unsigned int b = 0; b < allocationSize; b++)
            {
                if (block[b] != pattern)
                {
                    corruptedBlocks++;
                    break;
                }
            }
            addresses.push_back((std::uintptr_t)pointer);
        }
    }

    std::sort(addresses.begin(), addresses.end());
    for (size_t i = 1; i < addresses.size(); i++)
        if (addresses[i - 1] + allocationSize > addresses[i])
            overlappingBlocks++;

    if (allocatorType == AllocatorType::Stack)
    {
        allocator.Clear();
    }
    else
    {
        for (int threadId = 0; threadId < numberOfThreads; threadId++)
            for (void* pointer : threadBlocks[threadId])
                if (pointer != nullptr)
                    allocator.Free(pointer);
    }

    const bool passed = failedAllocations == 0 && corruptedBlocks == 0 && overlappingBlocks == 0;
    std::cout << AllocatorName(allocatorType) << " thread test: " << (passed ? "PASS" : "FAIL") << "\n";
    std::cout << "  failed allocations: " << failedAllocations << "\n";
    std::cout << "  corrupted blocks: "   << corruptedBlocks   << "\n";
    std::cout << "  overlapping blocks: " << overlappingBlocks << "\n";
}