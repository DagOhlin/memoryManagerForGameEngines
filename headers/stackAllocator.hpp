#pragma once

#include "allocator.hpp"

class StackAllocator : public Allocator
{
public:
    StackAllocator() = default;
    ~StackAllocator() = default;

    virtual void Initialize(unsigned int size); // Initializes allocator
    virtual void *Allocate(unsigned int size);  // Allocates memory
    virtual void Free(void *pointer);           // Frees memory
    virtual void Clear();                       // Frees all associated data
};