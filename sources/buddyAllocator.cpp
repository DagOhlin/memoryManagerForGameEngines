#include "buddyAllocator.hpp"

BuddyBlock::BuddyBlock()
{
}

BuddyBlock::BuddyBlock(void *base, unsigned int size)
{
    this->base = (char*)base;
    this->size = size;
}

void BuddyAllocator::Initialize(unsigned int size)
{
    this->size = NextPow2(size);
    this->nrOfStacks = 64 - __builtin_clzl(this->size);

    for (int i = 0; i < nrOfStacks; i++)
    {
        std::vector<BuddyBlock> currentBlockSizeArray;
        this->freeBuddyBlocks.push_back(currentBlockSizeArray);
    }

    this->base = malloc(this->size);
    this->freeBuddyBlocks.back().push_back(BuddyBlock(this->base, this->size));
}

void *BuddyAllocator::Allocate(unsigned int size)
{
    if(size <= 0) return nullptr;

    unsigned int actualSize = NextPow2(size + 4);
    unsigned int targetStackIndex = 64 - (__builtin_clzl(actualSize) + 1);
    unsigned int currentStackIndex = targetStackIndex;

    while(currentStackIndex < this->nrOfStacks &&
        this->freeBuddyBlocks[currentStackIndex].size() == 0)
    {
        currentStackIndex++;
    }

    if(currentStackIndex >= this->nrOfStacks)
    {
        std::cout << "Out of memory exception!" << std::endl;
        return nullptr;
    }
    
    BuddyBlock currentBlock = this->freeBuddyBlocks[currentStackIndex].back();
    this->freeBuddyBlocks[currentStackIndex].pop_back();

    while(currentStackIndex > targetStackIndex)
    {
        currentBlock.size /= 2;

        this->freeBuddyBlocks[currentStackIndex - 1].push_back(
            BuddyBlock(currentBlock.base + currentBlock.size, currentBlock.size));

        currentStackIndex -= 1;
    }

    //Store the block size, a 32-bit integer, just infront of the memory to be able to deallocate later
    uint32_t sizePayload = currentBlock.size;
    uint32_t* memoryPosition = (uint32_t*)currentBlock.base;
    *memoryPosition = sizePayload;

    return (void*)(currentBlock.base + 4);
}

void BuddyAllocator::Free(void *pointer)
{
    //the real base position is 32 bits in front of the pointer
    uint32_t* memoryPosition = (uint32_t*)pointer - 1;
    uint32_t blockSize = *memoryPosition;
    BuddyBlock freeBlock(memoryPosition, blockSize);

    void* buddyMemoryPosition = (void*)((uint32_t)memoryPosition ^ blockSize);
    unsigned int targetStackIndex = 64 - (__builtin_clzl((uint64_t)blockSize) + 1);

    int i = 0;
    bool foundBlock = false;
    //Loop through the target stack to look if buddy is free, merge if true
    while(!foundBlock && i < this->freeBuddyBlocks[targetStackIndex].size())
    {
        //The buddy was free
        if(buddyMemoryPosition == (uint32_t*)freeBuddyBlocks[targetStackIndex][i].base)
        {
            foundBlock = true;
            freeBuddyBlocks[targetStackIndex].erase(freeBuddyBlocks[targetStackIndex].begin() + i);
            freeBlock.size *= 2;

            //If merging with the buddy size gets 2x bigger => targetStackIndex gets increased
            targetStackIndex++;
            
            //The buddy was the left block, now freeBlock adress should instead be buddyMemoryPosition
            if((uint32_t*)buddyMemoryPosition < memoryPosition)
            {
                freeBlock.base = (char*)buddyMemoryPosition;
            }
        }

        i++;
    }
}

void BuddyAllocator::Clear()
{
}

uint64_t BuddyAllocator::NextPow2(unsigned int number)
{
    //__bultin_clzl returns the number of leading 0's before the 1st 1 in the binary number
    return number == 1 ? 1 : 1 << (64 - __builtin_clzl(number - 1));
}
