#pragma once
#include "allocator.hpp"

class StackAllocator : public Allocator
{
public:
    StackAllocator() = default;
    ~StackAllocator() = default;

    StackAllocator(const StackAllocator &) = delete;
    StackAllocator &operator=(const StackAllocator &) = delete;

    void Initialize(unsigned int size) override; // Reserves le backing buffer
    void *Allocate(unsigned int size) override; // Pushes a block; nullptr if out of space
    void Free(void *pointer) override; // Pops this block
    void Clear() override;                       // Resets the stack; keeps the buffer

private:
    struct Header
    {
        std::size_t prevOffset; // top of stack before this allocation
    };

    static constexpr std::size_t kAlignment = 16;

    unsigned char *m_start = nullptr;
    std::size_t m_size = 0;
    std::size_t m_offset = 0; // current top of stack
};