#pragma once

#include "IBoard.h"
#include "Piece.h"

#include <optional>
#include <vector>

class Board : public IBoard
{
public:
    Board();

    const IPiece& GetPiece(PieceId id) const override;
    EMoveResult CheckMove(PieceId id, int diceValue) const override;
    EMoveResult MovePiece(PieceId id, int diceValue) override;
    std::vector<int> GetMovablePieces(EColor color, int diceValue) const override;
    bool AreAllPiecesHome(EColor color) const override;
    std::optional<PieceId> GetLastCapturedPiece() const override;
    void Reset() override;

private:
    static bool IsValidPieceId(PieceId id);
    static bool IsValidDiceValue(int diceValue);
    static int ToStorageIndex(PieceId id);

    Piece& GetPieceRef(PieceId id);
    const Piece& GetPieceRef(PieceId id) const;

    static int ComputeTargetProgress(const Piece& piece, int diceValue);

    bool IsOccupiedByOwnPiece(EColor color, int targetProgress) const;
    std::optional<PieceId> FindOpponentOnTrackSquare(EColor movingColor, int trackSquare) const;

    std::vector<Piece> m_pieces;
    std::optional<PieceId> m_lastCapturedPiece;
};
