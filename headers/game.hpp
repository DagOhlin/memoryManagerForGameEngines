#pragma once

#include "allocator.hpp"
#include "common.hpp"

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

};