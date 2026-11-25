#pragma once

#include "allocator.hpp"

class PoolAllocator : public Allocator
{
public:
    PoolAllocator() = default;
    ~PoolAllocator() = default;

    virtual void Initialize(unsigned int size) override;                      // Initializes allocator
    virtual void Initialize(unsigned int chunkSize, unsigned int chunkCount); // Initializes the pool allocator
    virtual void *Allocate(unsigned int size) override;                       // Allocates memory
    virtual void *Allocate();                                                 // Returns a chunk of memory
    virtual void Free(void *pointer) override;                                // Frees memory
    virtual void Clear() override;                                            // Frees all associated data

private:
    void SetupLinkedList();

    struct FreeChunk
    {
        FreeChunk *next;
    };

    char *memory = nullptr;
    FreeChunk *nextFreeChunk = nullptr;

    unsigned int chunkSize = 0;
    unsigned int chunkCount = 0;

    const static unsigned int defaultChunkSize = 64;
};