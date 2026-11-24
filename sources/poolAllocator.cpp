#include "poolAllocator.hpp"

#include <cassert>

void PoolAllocator::Initialize(unsigned int size)
{
    // Make sure the size variable is valid
    assert(size % PoolAllocator::blockSize == 0);

    const unsigned int chunkCount = size / PoolAllocator::blockSize;
    this->chunkArray = this->nextFreeChunk = new Chunk[chunkCount];

    // Put together the linked list
    for (unsigned int i = 0; i < chunkCount - 1; i++)
    {
        this->chunkArray[i].next = &this->chunkArray[i + 1];
    }
    this->chunkArray[chunkCount - 1].next = nullptr;
}

void *PoolAllocator::Allocate(unsigned int size)
{
    if (this->chunkArray == nullptr || this->nextFreeChunk == nullptr)
    {
        std::cout << "Pool Allocator is full" << std::endl;
        return nullptr;
    }

    if (size > PoolAllocator::blockSize)
    {
        std::cout << "Attempting to allocate larger than pool size" << std::endl;
        return nullptr;
    }

    Chunk *temp = this->nextFreeChunk;
    this->nextFreeChunk = this->nextFreeChunk->next;
    return temp;
}

void PoolAllocator::Free(void *pointer)
{
    if (this->chunkArray == nullptr)
    {
        std::cerr << "ERROR: Freeing in uninitialized pool" << std::endl;
        return;
    }

    if (pointer == nullptr)
    {
        std::cerr << "ERROR: Trying to free nullptr" << std::endl;
        return;
    }

    Chunk *freed = (Chunk *)pointer;
    freed->next = this->nextFreeChunk;
    this->nextFreeChunk = freed;
}

void PoolAllocator::Clear()
{
    if (this->chunkArray != nullptr)
    {
        delete[] this->chunkArray;
    }

    this->chunkArray = nullptr;
    this->nextFreeChunk = nullptr;
}
