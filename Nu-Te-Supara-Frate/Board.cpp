#include "Board.h"

#include "BoardConstants.h"

#include <stdexcept>

using namespace BoardConstants;

Board::Board()
{
    m_pieces.reserve(kPlayerCount * kPiecesPerPlayer);
    for (int colorIndex = 0; colorIndex < kPlayerCount; ++colorIndex)
    {
        for (int pieceIndex = 0; pieceIndex < kPiecesPerPlayer; ++pieceIndex)
        {
            m_pieces.emplace_back(static_cast<EColor>(colorIndex), pieceIndex);
        }
    }
}

const IPiece& Board::GetPiece(PieceId id) const
{
    return GetPieceRef(id);
}

EMoveResult Board::CheckMove(PieceId id, int diceValue) const
{
    if (!IsValidPieceId(id))
    {
        return EMoveResult::InvalidPieceId;
    }
    if (!IsValidDiceValue(diceValue))
    {
        return EMoveResult::InvalidDiceValue;
    }

    const Piece& piece = GetPieceRef(id);
    if (piece.GetState() == EPieceState::InBase && diceValue != kDiceValueToLeaveBase)
    {
        return EMoveResult::NeedsSixToLeaveBase;
    }

    const int targetProgress = ComputeTargetProgress(piece, diceValue);
    if (targetProgress > kLastHomeProgress)
    {
        return EMoveResult::OvershootsHome;
    }
    if (IsOccupiedByOwnPiece(id.color, targetProgress))
    {
        return EMoveResult::BlockedByOwnPiece;
    }

    const int targetSquare = ProgressToTrackSquare(id.color, targetProgress);
    if (FindOpponentOnTrackSquare(id.color, targetSquare).has_value())
    {
        return EMoveResult::Captured;
    }
    return EMoveResult::Moved;
}

EMoveResult Board::MovePiece(PieceId id, int diceValue)
{
    const EMoveResult result = CheckMove(id, diceValue);
    if (!IsSuccessfulMove(result))
    {
        return result;
    }

    Piece& piece = GetPieceRef(id);
    const int targetProgress = ComputeTargetProgress(piece, diceValue);

    m_lastCapturedPiece.reset();
    if (result == EMoveResult::Captured)
    {
        const int targetSquare = ProgressToTrackSquare(id.color, targetProgress);
        const std::optional<PieceId> capturedId = FindOpponentOnTrackSquare(id.color, targetSquare);
        GetPieceRef(*capturedId).ReturnToBase();
        m_lastCapturedPiece = capturedId;
    }

    piece.SetProgress(targetProgress);
    return result;
}

std::vector<int> Board::GetMovablePieces(EColor color, int diceValue) const
{
    std::vector<int> movablePieces;
    for (int pieceIndex = 0; pieceIndex < kPiecesPerPlayer; ++pieceIndex)
    {
        if (IsSuccessfulMove(CheckMove({ color, pieceIndex }, diceValue)))
        {
            movablePieces.push_back(pieceIndex);
        }
    }
    return movablePieces;
}

bool Board::AreAllPiecesHome(EColor color) const
{
    if (!IsValidPieceId({ color, 0 }))
    {
        return false;
    }
    for (int pieceIndex = 0; pieceIndex < kPiecesPerPlayer; ++pieceIndex)
    {
        if (GetPieceRef({ color, pieceIndex }).GetState() != EPieceState::InHome)
        {
            return false;
        }
    }
    return true;
}

std::optional<PieceId> Board::GetLastCapturedPiece() const
{
    return m_lastCapturedPiece;
}

void Board::Reset()
{
    for (Piece& piece : m_pieces)
    {
        piece.ReturnToBase();
    }
    m_lastCapturedPiece.reset();
}

bool Board::IsValidPieceId(PieceId id)
{
    const int colorIndex = static_cast<int>(id.color);
    return colorIndex >= 0 && colorIndex < kPlayerCount
        && id.index >= 0 && id.index < kPiecesPerPlayer;
}

bool Board::IsValidDiceValue(int diceValue)
{
    return diceValue >= kMinDiceValue && diceValue <= kMaxDiceValue;
}

int Board::ToStorageIndex(PieceId id)
{
    return static_cast<int>(id.color) * kPiecesPerPlayer + id.index;
}

Piece& Board::GetPieceRef(PieceId id)
{

    return const_cast<Piece&>(static_cast<const Board&>(*this).GetPieceRef(id));
}

const Piece& Board::GetPieceRef(PieceId id) const
{
    if (!IsValidPieceId(id))
    {
        throw std::out_of_range("Board: id de pion invalid");
    }
    return m_pieces[ToStorageIndex(id)];
}

int Board::ComputeTargetProgress(const Piece& piece, int diceValue)
{
    if (piece.GetState() == EPieceState::InBase)
    {
        return kStartProgress;
    }
    return piece.GetProgress() + diceValue;
}

bool Board::IsOccupiedByOwnPiece(EColor color, int targetProgress) const
{
    for (int pieceIndex = 0; pieceIndex < kPiecesPerPlayer; ++pieceIndex)
    {
        if (GetPieceRef({ color, pieceIndex }).GetProgress() == targetProgress)
        {
            return true;
        }
    }
    return false;
}

std::optional<PieceId> Board::FindOpponentOnTrackSquare(EColor movingColor, int trackSquare) const
{
    if (trackSquare == kNoSquare)
    {
        return std::nullopt;
    }
    for (const Piece& piece : m_pieces)
    {
        if (piece.GetColor() != movingColor && piece.GetTrackSquare() == trackSquare)
        {
            return PieceId{ piece.GetColor(), piece.GetIndex() };
        }
    }
    return std::nullopt;
}
