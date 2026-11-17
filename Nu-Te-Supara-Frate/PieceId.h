#pragma once

#include "EColor.h"

struct PieceId
{
    EColor color;
    int index;

    bool operator==(const PieceId& other) const
    {
        return color == other.color && index == other.index;
    }

    bool operator!=(const PieceId& other) const
    {
        return !(*this == other);
    }
};
