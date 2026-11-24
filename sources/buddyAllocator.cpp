#include "buddyAllocator.hpp"

BuddyBlock::BuddyBlock()
{
}

BuddyBlock::BuddyBlock(void *base, unsigned int size)
{
    this->base = base;
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
    this->freeBuddyBlocks[0].push_back(BuddyBlock(this->base, this->size));
}

void *BuddyAllocator::Allocate(unsigned int size)
{
    if(size <= 0) return nullptr;

    unsigned int actualSize = NextPow2(size);
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

    while(currentStackIndex > targetStackIndex)
    {
        currentBlock.size /= 2;

        this->freeBuddyBlocks[currentStackIndex - 1].push_back(BuddyBlock(currentBlock.base + currentBlock.size, currentBlock.size));
        this->freeBuddyBlocks[currentStackIndex].pop_back();

        currentStackIndex -= 1;
    }

    return currentBlock.base;
}

void BuddyAllocator::Free(void *pointer)
{

}

uint64_t BuddyAllocator::NextPow2(unsigned int number)
{
    //__bultin_clzl returns the number of leading 0's before the 1st 1 in the binary number
    return number == 1 ? 1 : 1 << (64 - __builtin_clzl(number - 1));
}
