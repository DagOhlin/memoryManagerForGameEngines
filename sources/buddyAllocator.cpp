#include "buddyAllocator.hpp"

#include <bit>
#include <cstdint>

BuddyBlock::BuddyBlock()
{
}

BuddyBlock::BuddyBlock(void *base, unsigned int size)
{
    this->base = (char *)base;
    this->size = size;
}

void BuddyAllocator::Initialize(unsigned int size)
{
#ifdef MULTITHREAD_SUPPORT
    this->buddyAllocatorLock.lock();
#endif

    this->size = NextPow2(size);
    this->nrOfStacks = GetStackIndex(this->size) + 1;
    // this->nrOfStacks = 64 - __builtin_clzl(this->size);

    for (int i = 0; i < nrOfStacks; i++)
    {
        this->freeBuddyBlocks.emplace_back().reserve(16);
    }

    this->base = malloc(this->size);
    this->freeBuddyBlocks.back().push_back(BuddyBlock(this->base, this->size));

#ifdef MULTITHREAD_SUPPORT
    this->buddyAllocatorLock.unlock();
#endif
}

void *BuddyAllocator::Allocate(unsigned int size)
{
#ifdef MULTITHREAD_SUPPORT
    this->buddyAllocatorLock.lock();
#endif

    if (size <= 0)
    {
#ifdef MULTITHREAD_SUPPORT
        this->buddyAllocatorLock.unlock();
#endif

        return nullptr;
    }

    unsigned int actualSize = NextPow2(size + 8);
    unsigned int targetStackIndex = GetStackIndex(actualSize);
    // unsigned int targetStackIndex = 64 - (__builtin_clzl(actualSize) + 1);
    unsigned int currentStackIndex = targetStackIndex;

    while (currentStackIndex < this->nrOfStacks &&
           this->freeBuddyBlocks[currentStackIndex].size() == 0)
    {
        currentStackIndex++;
    }

    if (currentStackIndex >= this->nrOfStacks)
    {
        std::cout << "Out of memory exception!" << std::endl;

#ifdef MULTITHREAD_SUPPORT
        this->buddyAllocatorLock.unlock();
#endif

        return nullptr;
    }

    BuddyBlock currentBlock = this->freeBuddyBlocks[currentStackIndex].back();
    this->freeBuddyBlocks[currentStackIndex].pop_back();

    while (currentStackIndex > targetStackIndex)
    {
        currentBlock.size /= 2;

        this->freeBuddyBlocks[currentStackIndex - 1].push_back(
            BuddyBlock(currentBlock.base + currentBlock.size, currentBlock.size));

        currentStackIndex -= 1;
    }

    // Store the block size, a 64-bit integer, just infront of the memory to be able to deallocate later
    uint64_t sizePayload = currentBlock.size;
    uint64_t *memoryPosition = (uint64_t *)currentBlock.base;
    *memoryPosition = sizePayload;

#ifdef MULTITHREAD_SUPPORT
    this->buddyAllocatorLock.unlock();
#endif

    return (void *)(currentBlock.base + 8);
}

void BuddyAllocator::Free(void *pointer)
{
#ifdef MULTITHREAD_SUPPORT
    this->buddyAllocatorLock.lock();
#endif

    // Trying to dealloc outside of the buddy-pool
    //  if((uint64_t)pointer < (uint64_t)this->base || (uint64_t)pointer >= (uint64_t)this->base + (uint64_t)this->size)
    //  {
    //      std::cout << "trying to dealloc outside the buddy-pool! Operation ignored" << std::endl;
    //      return;
    //  }

    // the real base position is 64 bits in front of the pointer
    uint64_t *memoryPosition = (uint64_t *)pointer - 1;
    uint64_t blockSize = *memoryPosition;
    BuddyBlock freeBlock(memoryPosition, blockSize);

    void *buddyMemoryPosition = (void *)((uint64_t)memoryPosition ^ blockSize);
    unsigned int targetStackIndex = GetStackIndex(blockSize);
    // unsigned int targetStackIndex = 64 - (__builtin_clzl((uint64_t)blockSize) + 1);

    int i = 0;
    bool foundBlock = false;
    // Loop through the target stack to look if buddy is free, merge if true
    while (!foundBlock && i < this->freeBuddyBlocks[targetStackIndex].size())
    {
        // The buddy was free
        if (buddyMemoryPosition == (uint64_t *)freeBuddyBlocks[targetStackIndex][i].base)
        {
            foundBlock = true;
            freeBuddyBlocks[targetStackIndex].erase(freeBuddyBlocks[targetStackIndex].begin() + i);
            freeBlock.size *= 2;

            // If merging with the buddy size gets 2x bigger => targetStackIndex gets increased
            targetStackIndex++;

            // The buddy was the left block, now freeBlock adress should instead be buddyMemoryPosition
            if ((uint64_t *)buddyMemoryPosition < memoryPosition)
            {
                freeBlock.base = (char *)buddyMemoryPosition;
            }
        }

        i++;
    }

    this->freeBuddyBlocks[targetStackIndex].push_back(freeBlock);

#ifdef MULTITHREAD_SUPPORT
    this->buddyAllocatorLock.unlock();
#endif
}

void BuddyAllocator::Clear()
{
#ifdef MULTITHREAD_SUPPORT
    this->buddyAllocatorLock.lock();
#endif

    free(this->base);

    this->freeBuddyBlocks.clear();
    this->base = nullptr;
    this->size = 0;
    this->nrOfStacks = 0;

#ifdef MULTITHREAD_SUPPORT
    this->buddyAllocatorLock.unlock();
#endif
}

uint64_t BuddyAllocator::NextPow2(uint64_t number)
{
    // Return next power of 2
    return number <= 1 ? 1 : std::bit_ceil(number);
    //__bultin_clzl returns the number of leading 0's before the 1st 1 in the binary number
    // return number == 1 ? 1 : 1 << (64 - __builtin_clzl(number - 1));
}

uint64_t BuddyAllocator::GetStackIndex(uint64_t number)
{
    // Calculates the stack index based on how many bits it takes to store the number
    return std::bit_width(number) - 1;
}

void BuddyAllocator::PrintState()
{
    std::cout << "---------Nr-of-byte-blocks---------" << std::endl;

    for (int i = 0; i < this->nrOfStacks; i++)
    {
        std::cout << pow(2, i) << "byte blocks: " << this->freeBuddyBlocks[i].size() << std::endl;
    }

    std::cout << "-----------------------------------" << std::endl;
}

void BuddyAllocatorTester::SmallTest()
{
    BuddyAllocator buddy;
    buddy.Initialize(253);
    buddy.PrintState();

    int *test1 = (int *)buddy.Allocate(sizeof(int));
    buddy.PrintState();

    buddy.Free(test1);
    buddy.PrintState();

    std::cout << "********************************************************" << std::endl;

    test1 = (int *)buddy.Allocate(sizeof(int));
    buddy.PrintState();

    char *test2 = (char *)buddy.Allocate(8); // 8 characters
    buddy.PrintState();

    buddy.Free((void *)test2);
    buddy.PrintState();

    buddy.Free(test1);
    buddy.PrintState();

    std::cout << "********************************************************" << std::endl;

    std::string word = "AbCdEfGhAbCdEfG";

    test1 = (int *)buddy.Allocate(8);
    buddy.PrintState();

    test2 = (char *)buddy.Allocate(8); // 8 characters
    buddy.PrintState();

    char *test3 = (char *)buddy.Allocate(16); // 16 characters
    buddy.PrintState();

    int *test4 = (int *)buddy.Allocate(8);
    buddy.PrintState();

    *test1 = 123456;

    for (int i = 0; i < 7; i++)
    {
        test2[i] = word[i];
    }
    test2[7] = '\0';

    for (int i = 0; i < 15; i++)
    {
        test3[i] = word[i];
    }
    test3[15] = '\0';

    *test4 = 6969;

    std::cout << "test1: " << *test1 << std::endl;
    std::cout << "test2: " << test2 << std::endl;
    std::cout << "test3: " << test3 << std::endl;
    std::cout << "test4: " << *test4 << std::endl;

    buddy.Free(test1);
    buddy.PrintState();

    buddy.Free((void *)test2);
    buddy.PrintState();

    buddy.Free((void *)test3);
    buddy.PrintState();

    buddy.Free(test4);
    buddy.PrintState();
}

void BuddyAllocatorTester::StressTest()
{
    std::cout << "------Starting-Stress-Test------" << std::endl;

    BuddyAllocator buddy;
    buddy.Initialize(32 * 1000);
    buddy.PrintState();

    std::cout << "------Init-Done------" << std::endl;

    std::vector<uint32_t *> integers;

    for (int i = 0; i < 512; i++)
    {
        uint32_t *integer = (uint32_t *)buddy.Allocate(32);
        *integer = i;
        integers.push_back(integer);
    }

    // This should fail because no memory left
    std::cout << "should fail because no memory left" << std::endl;
    uint32_t *integer = (uint32_t *)buddy.Allocate(32);
    integers.push_back(integer);

    for (int i = 0; i < 512; i++)
    {
        std::cout << *integers[i] << std::endl;
    }

    if (integers[512] == nullptr)
    {
        std::cout << "Nice, stray integer didnt get allocated" << std::endl;
    }
    else
    {
        std::cout << "Error! Somehow stray integer got allocated anyways" << std::endl;
    }

    buddy.PrintState();
    std::cout << "------Alloc-Done------" << std::endl;

    for (int i = 0; i < 512; i++)
    {
        buddy.Free(integers[i]);
    }

    buddy.PrintState();
    std::cout << "------Free-Done------" << std::endl;

    integers.clear();

    for (int i = 0; i < 512; i++)
    {
        uint32_t *integer = (uint32_t *)buddy.Allocate(32);
        *integer = i;
        integers.push_back(integer);
    }

    // This should fail because no memory left
    std::cout << "should fail because no memory left" << std::endl;
    integer = (uint32_t *)buddy.Allocate(32);
    integers.push_back(integer);

    for (int i = 0; i < 512; i++)
    {
        std::cout << *integers[i] << std::endl;
    }

    if (integers[512] == nullptr)
    {
        std::cout << "Nice, stray integer didnt get allocated" << std::endl;
    }
    else
    {
        std::cout << "Error! Somehow stray integer got allocated anyways" << std::endl;
    }

    buddy.PrintState();
    std::cout << "------Alloc2-Done------" << std::endl;

    for (int i = 0; i < 512; i++)
    {
        buddy.Free(integers[i]);
    }

    buddy.PrintState();
    std::cout << "------Free2-Done------" << std::endl;

    buddy.Clear();
    std::cout << "------Clear-Done------" << std::endl;
}
