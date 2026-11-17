#pragma once

#include "EColor.h"
#include "EPieceState.h"

class IPiece
{
public:
    virtual ~IPiece() = default;

    virtual EColor GetColor() const = 0;
    virtual int GetIndex() const = 0;
    virtual EPieceState GetState() const = 0;

    virtual int GetProgress() const = 0;

    virtual int GetTrackSquare() const = 0;

    virtual int GetHomeSquare() const = 0;
};
