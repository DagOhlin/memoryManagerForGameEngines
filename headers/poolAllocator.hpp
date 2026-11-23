#pragma once

#include "allocator.hpp"

class PoolAllocator : public Allocator
{
public:
    PoolAllocator() = default;
    ~PoolAllocator() = default;

    virtual void Initialize(unsigned int size); // Initializes allocator
    virtual void *Allocate(unsigned int size);  // Allocates memory
    virtual void Free(void *pointer);           // Frees memory
    virtual void Clear();                       // Frees all associated data
};