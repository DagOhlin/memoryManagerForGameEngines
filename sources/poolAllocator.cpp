#include "poolAllocator.hpp"

#include <cassert>
#include <string>

void PoolAllocator::Initialize(unsigned int size)
{
    this->Clear();

    // Make sure the size variable is valid
    assert(size % PoolAllocator::defaultChunkSize == 0);

    // Use default chunk size
    const unsigned int chunkCount = size / PoolAllocator::defaultChunkSize;
    this->chunkSize = PoolAllocator::defaultChunkSize;
    this->chunkCount = chunkCount;

    this->memory = new char[this->chunkCount * this->chunkSize];

    this->SetupLinkedList();
}

void PoolAllocator::Initialize(unsigned int chunkSize, unsigned int chunkCount)
{
    this->Clear();

    if (chunkSize < sizeof(FreeChunk))
    {
        std::cerr << "ERROR: chunkSize must be larger than " << std::to_string(sizeof(FreeChunk)) << " bytes" << std::endl;
        return;
    }

    this->chunkSize = chunkSize;
    this->chunkCount = chunkCount;

    this->memory = new char[this->chunkCount * this->chunkSize];

    this->SetupLinkedList();
}

void *PoolAllocator::Allocate(unsigned int size)
{
    if (size > this->chunkSize)
    {
        std::cerr << "ERROR: Attempting to allocate larger than pool size" << std::endl;
        return nullptr;
    }

    return this->Allocate();
}

void *PoolAllocator::Allocate()
{
    if (this->memory == nullptr)
    {
        std::cerr << "ERROR: Allocating in uninitialized pool" << std::endl;
        return nullptr;
    }

    if (this->nextFreeChunk == nullptr)
    {
        std::cout << "Pool Allocator is full" << std::endl;
        return nullptr;
    }

    FreeChunk *temp = this->nextFreeChunk;
    this->nextFreeChunk = this->nextFreeChunk->next;
    return temp;
}

void PoolAllocator::Free(void *pointer)
{
    if (this->memory == nullptr)
    {
        std::cerr << "ERROR: Freeing in uninitialized pool" << std::endl;
        return;
    }

    if (pointer == nullptr)
    {
        std::cerr << "ERROR: Trying to free nullptr" << std::endl;
        return;
    }

    // This might change the order of allocations
    // but it doesn't matter as long as we only ever return one block
    FreeChunk *freed = (FreeChunk *)pointer;
    freed->next = this->nextFreeChunk;
    this->nextFreeChunk = freed;
}

void PoolAllocator::Clear()
{
    if (this->memory != nullptr)
    {
        delete[] this->memory;
    }

    this->memory = nullptr;
    this->nextFreeChunk = nullptr;

    this->chunkSize = 0;
    this->chunkCount = 0;
}

void PoolAllocator::SetupLinkedList()
{
    // Since using unsigned ints, having this->chunkCount = 0 means this->chunkCount - 1 will be very large
    assert(chunkCount > 0);

    // Put together the linked list
    for (unsigned int i = 0; i < this->chunkCount - 1; i++)
    {
        ((FreeChunk *)&this->memory[i * this->chunkSize])->next = (FreeChunk *)&this->memory[(i + 1) * this->chunkSize];
    }
    ((FreeChunk *)&this->memory[(this->chunkCount - 1) * this->chunkSize])->next = nullptr;
    this->nextFreeChunk = (FreeChunk *)this->memory;
}
