#pragma once

/*
Allocator Interface
*/
class Allocator
{
public:
    Allocator() = default;
    ~Allocator() = default;

    virtual void Initialize(unsigned int size) = 0; // Initializes allocator
    virtual void *Allocate(unsigned int size) = 0;  // Allocates memory
    virtual void Free(void *pointer) = 0;           // Frees memory
    virtual void Clear() = 0;                       // Frees all associated data
};