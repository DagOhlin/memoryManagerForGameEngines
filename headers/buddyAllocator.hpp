#pragma once

#include "allocator.hpp"

class BuddyAllocator : public Allocator
{
public:
    BuddyAllocator() = default;
    ~BuddyAllocator() = default;

    virtual void Initialize(unsigned int size); // Initializes allocator
    virtual void *Allocate(unsigned int size);  // Allocates memory
    virtual void Free(void *pointer);           // Frees memory
    virtual void Clear();                       // Frees all associated data
};