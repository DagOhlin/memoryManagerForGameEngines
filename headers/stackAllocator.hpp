#pragma once

#include "allocator.hpp"

// #define MULTITHREAD_SUPPORT

#ifdef MULTITHREAD_SUPPORT
#include <mutex>
#endif

class StackAllocator : public Allocator
{
public:
    StackAllocator() = default;
    ~StackAllocator() = default;

    StackAllocator(const StackAllocator &) = delete;
    StackAllocator &operator=(const StackAllocator &) = delete;

    virtual void Initialize(unsigned int size) override; // Initializes allocator
    virtual void *Allocate(unsigned int size) override; // Pushes a block onto the stack
    virtual void Free(void *pointer) override; // Pops this block & all above it
    virtual void Clear() override; // Frees all associated data
    virtual void Reset(); // Rewinds stack & keep memory

private:
    char *memory = nullptr;
    unsigned int capacity = 0;
    unsigned int offset = 0; // current top of the stack

    static constexpr unsigned int alignment = 16;

#ifdef MULTITHREAD_SUPPORT
    std::mutex stackAllocatorLock;
#endif
};