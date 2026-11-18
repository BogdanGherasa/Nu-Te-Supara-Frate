#pragma once

#include "EColor.h"


namespace BoardConstants
{
    constexpr int kPlayerCount = 4;
    constexpr int kPiecesPerPlayer = 4;

    constexpr int kTrackLength = 40;
    constexpr int kHomeLength = 4;
    constexpr int kSquaresBetweenStarts = kTrackLength / kPlayerCount;

    constexpr int kMinDiceValue = 1;
    constexpr int kMaxDiceValue = 6;
    constexpr int kDiceValueToLeaveBase = 6;

    constexpr int kBaseProgress = -1;
    constexpr int kStartProgress = 0;
    constexpr int kLastTrackProgress = kTrackLength - 1;
    constexpr int kFirstHomeProgress = kTrackLength;
    constexpr int kLastHomeProgress = kTrackLength + kHomeLength - 1;

    constexpr int kNoSquare = -1;

    constexpr int GetStartSquare(EColor color)
    {
        return static_cast<int>(color) * kSquaresBetweenStarts;
    }

    constexpr int ProgressToTrackSquare(EColor color, int progress)
    {
        if (progress < kStartProgress || progress > kLastTrackProgress)
        {
            return kNoSquare;
        }
        return (GetStartSquare(color) + progress) % kTrackLength;
    }

    constexpr int ProgressToHomeSquare(int progress)
    {
        if (progress < kFirstHomeProgress || progress > kLastHomeProgress)
        {
            return kNoSquare;
        }
        return progress - kFirstHomeProgress;
    }
}