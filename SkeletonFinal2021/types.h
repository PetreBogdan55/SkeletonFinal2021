#pragma once
#include <vector>

// TODO: Create possible Directions 

enum class MoveDirection : uint8_t
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
};
using Moves = std::vector<MoveDirection>;
