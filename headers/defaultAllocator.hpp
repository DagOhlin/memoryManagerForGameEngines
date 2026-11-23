#pragma once

#include "allocator.hpp"

#include <stdlib.h>

class DefaultAllocator : public Allocator
{
public:
    DefaultAllocator() = default;
    ~DefaultAllocator() = default;

    virtual void Initialize(unsigned int size) {}                      // Initializes allocator
    virtual void *Allocate(unsigned int size) { return malloc(size); } // Allocates memory
    virtual void Free(void *pointer) { free(pointer); }                // Frees memory
    virtual void Clear() {}                                            // Frees all associated data
};