#pragma once

#include <cstdint>

enum class EMoveResult : uint8_t
{
    Moved,
    Captured,
    InvalidPieceId,
    InvalidDiceValue,
    NeedsSixToLeaveBase,
    BlockedByOwnPiece,
    OvershootsHome
};

constexpr bool IsSuccessfulMove(EMoveResult result)
{
    return result == EMoveResult::Moved || result == EMoveResult::Captured;
}
