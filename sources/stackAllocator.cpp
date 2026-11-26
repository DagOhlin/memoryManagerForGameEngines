// stackAllocator.cpp
#include "stackAllocator.hpp"

#include <cassert>
#include <iostream>

void StackAllocator::Initialize(unsigned int size)
{
    this->Clear();

#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.lock();
#endif

    assert(size > 0);

    this->memory = new char[size];
    this->capacity = size;
    this->offset = 0;

#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.unlock();
#endif
}

void *StackAllocator::Allocate(unsigned int size)
{
#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.lock();
#endif

    if (this->memory == nullptr)
    {
        std::cerr << "ERROR: Allocating in uninitialized stack" << std::endl;
#ifdef MULTITHREAD_SUPPORT
        this->stackAllocatorLock.unlock();
#endif
        return nullptr;
    }

    // Round the top up to alignment, new char[] is aligned for any type = aligned offset => aligned pointer.
    const unsigned int alignedOffset = (this->offset + alignment - 1) & ~(alignment - 1);

    // No Overflow here
    if (alignedOffset > this->capacity || size > this->capacity - alignedOffset)
    {
        std::cerr << "ERROR: Stack Allocator is full" << std::endl;
#ifdef MULTITHREAD_SUPPORT
        this->stackAllocatorLock.unlock();
#endif
        return nullptr;
    }

    void *result = this->memory + alignedOffset;
    this->offset = alignedOffset + size;

#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.unlock();
#endif

    return result;
}

void StackAllocator::Free(void *pointer)
{
#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.lock();
#endif

    if (this->memory == nullptr)
    {
        std::cerr << "ERROR: Freeing in uninitialized stack" << std::endl;
#ifdef MULTITHREAD_SUPPORT
        this->stackAllocatorLock.unlock();
#endif
        return;
    }

    if (pointer == nullptr)
    {
        std::cerr << "ERROR: Trying to free nullptr" << std::endl;
#ifdef MULTITHREAD_SUPPORT
        this->stackAllocatorLock.unlock();
#endif
        return;
    }

    char *p = static_cast<char *>(pointer);

    // Must point at a block that is still live
    if (p < this->memory || p >= this->memory + this->offset)
    {
        std::cerr << "ERROR: Pointer not owned by this stack (or already freed)" << std::endl;
#ifdef MULTITHREAD_SUPPORT
        this->stackAllocatorLock.unlock();
#endif
        return;
    }

    // Everything at and above this pointer is released
    this->offset = static_cast<unsigned int>(p - this->memory);

#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.unlock();
#endif
}

void StackAllocator::Clear()
{
#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.lock();
#endif

    if (this->memory != nullptr)
    {
        delete[] this->memory;
    }

    this->memory = nullptr;
    this->capacity = 0;
    this->offset = 0;

#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.unlock();
#endif
}

void StackAllocator::Reset()
{
#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.lock();
#endif

    this->offset = 0;

#ifdef MULTITHREAD_SUPPORT
    this->stackAllocatorLock.unlock();
#endif
}