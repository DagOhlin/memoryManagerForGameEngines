#pragma once

#include "allocator.hpp"
#include "common.hpp"
#include <random>
#include <algorithm>

enum class AllocatorType
{
    Default,
    Stack,
    Pool,
    Buddy
};

class Game
{
public:
    Game() = default;
    ~Game() = default;

    void Run();

private:

    void FrameAllocatorTestlevel1(Allocator& allocator, AllocatorType allocatorType);
    void FrameAllocatorTestlevel2(Allocator& allocator, AllocatorType allocatorType);

    void PoolAllocatorTestlevel1(Allocator& allocator, AllocatorType allocatorType);

};

inline const char* AllocatorName(AllocatorType type)
{
    switch (type)
    {
    case AllocatorType::Default: return "Default";
    case AllocatorType::Stack:   return "Stack";
    case AllocatorType::Pool:    return "Pool";
    case AllocatorType::Buddy:   return "Buddy";
    }
    return "Unknown";
}