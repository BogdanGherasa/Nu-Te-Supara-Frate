#pragma once

#include <cstdint>

enum class EGameState : uint8_t
{
    NotStarted,
    WaitingForRoll,
    WaitingForMove,
    Finished
};
