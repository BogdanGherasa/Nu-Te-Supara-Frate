#pragma once

#include "EColor.h"
#include "EMoveResult.h"
#include "IPiece.h"
#include "PieceId.h"

#include <optional>
#include <vector>

class IBoard
{
public:
    virtual ~IBoard() = default;

    virtual const IPiece& GetPiece(PieceId id) const = 0;

    virtual EMoveResult CheckMove(PieceId id, int diceValue) const = 0;

    virtual EMoveResult MovePiece(PieceId id, int diceValue) = 0;

    virtual std::vector<int> GetMovablePieces(EColor color, int diceValue) const = 0;

    virtual bool AreAllPiecesHome(EColor color) const = 0;

    virtual std::optional<PieceId> GetLastCapturedPiece() const = 0;

    virtual void Reset() = 0;
};