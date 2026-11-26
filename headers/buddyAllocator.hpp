#pragma once
#include <vector>
#include <cmath>
#include "allocator.hpp"

struct BuddyBlock
{
    BuddyBlock();
    BuddyBlock(void* base, unsigned int size);

    char* base = nullptr;
    unsigned int size = 0;
};


class BuddyAllocator : public Allocator
{
public:
    BuddyAllocator() = default;
    ~BuddyAllocator() = default;

    virtual void Initialize(unsigned int size); // Initializes allocator
    virtual void *Allocate(unsigned int size);  // Allocates memory
    virtual void Free(void *pointer);           // Frees memory
    virtual void Clear();                       // Frees all associated data
    
    uint64_t NextPow2(unsigned int number);
    void PrintState();

private:
    void* base = nullptr;
    unsigned int size = 0;
    unsigned int nrOfStacks = 0;

    std::vector<std::vector<BuddyBlock>> freeBuddyBlocks;
};

class BuddyAllocatorTester
{
public:
    BuddyAllocatorTester() = default;
    ~BuddyAllocatorTester() = default;

    void SmallTest();
    void StressTest();
};