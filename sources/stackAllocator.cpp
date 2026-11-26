// stack_allocator.cpp
#include "stackAllocator.hpp"

#include <cassert>

static std::uintptr_t AlignUp(std::uintptr_t value, std::size_t alignment)
{
    return (value + (alignment - 1)) & ~(static_cast<std::uintptr_t>(alignment) - 1);
}

StackAllocator::~StackAllocator()
{
    std::free(m_start);
}

void StackAllocator::Initialize(unsigned int size)
{
    std::free(m_start); // safe if re-initializing (free(nullptr) is a no-op)
    m_start = static_cast<unsigned char *>(std::malloc(size));
    assert(m_start && "StackAllocator: backing allocation failed");
    m_size = m_start ? size : 0;
    m_offset = 0;
}

void *StackAllocator::Allocate(unsigned int size)
{
    if (!m_start){return nullptr;}
    const std::uintptr_t current = reinterpret_cast<std::uintptr_t>(m_start) + m_offset;

    // Leave room for the header, then align the user pointer.
    const std::uintptr_t userAddr = AlignUp(current + sizeof(Header), kAlignment);
    const std::size_t newOffset = (userAddr - reinterpret_cast<std::uintptr_t>(m_start)) + size;

    if (newOffset > m_size) {return nullptr;} // out of memory

    Header *header = reinterpret_cast<Header *>(userAddr - sizeof(Header));
    header->prevOffset = m_offset;

    m_offset = newOffset;
    return reinterpret_cast<void *>(userAddr);
}

void StackAllocator::Free(void *pointer)
{
    if (!pointer)
        return;

    auto *p = static_cast<unsigned char *>(pointer);
    assert(p >= m_start + sizeof(Header) && p < m_start + m_offset &&
           "StackAllocator: pointer not owned by this stack (or already freed)");

    const Header *header = reinterpret_cast<const Header *>(p - sizeof(Header));
    m_offset = header->prevOffset;
}

void StackAllocator::Clear()
{
    m_offset = 0;
}