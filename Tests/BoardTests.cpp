#include "pch.h"

#include "Board.h"
#include "BoardConstants.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <stdexcept>

using namespace BoardConstants;

namespace
{
    const PieceId kRed0{ EColor::Red, 0 };
    const PieceId kRed1{ EColor::Red, 1 };
    const PieceId kRed2{ EColor::Red, 2 };
    const PieceId kRed3{ EColor::Red, 3 };
    const PieceId kBlue0{ EColor::Blue, 0 };
    const PieceId kYellow0{ EColor::Yellow, 0 };

    const EColor kInvalidColor = static_cast<EColor>(kPlayerCount);
}

class BoardTest : public ::testing::Test
{
protected:
    void PlaceOnProgress(PieceId id, int targetProgress)
    {
        ASSERT_EQ(board.GetPiece(id).GetState(), EPieceState::InBase);
        ASSERT_TRUE(IsSuccessfulMove(board.MovePiece(id, kDiceValueToLeaveBase)));

        int remainingSteps = targetProgress;
        while (remainingSteps > 0)
        {
            const int diceValue = std::min(remainingSteps, kMaxDiceValue);
            ASSERT_TRUE(IsSuccessfulMove(board.MovePiece(id, diceValue)));
            remainingSteps -= diceValue;
        }
        ASSERT_EQ(board.GetPiece(id).GetProgress(), targetProgress);
    }

    Board board;
};


TEST_F(BoardTest, NewBoardHasAllPiecesInBase)
{
    for (int colorIndex = 0; colorIndex < kPlayerCount; ++colorIndex)
    {
        for (int pieceIndex = 0; pieceIndex < kPiecesPerPlayer; ++pieceIndex)
        {
            const EColor color = static_cast<EColor>(colorIndex);
            const IPiece& piece = board.GetPiece({ color, pieceIndex });

            EXPECT_EQ(piece.GetColor(), color);
            EXPECT_EQ(piece.GetIndex(), pieceIndex);
            EXPECT_EQ(piece.GetState(), EPieceState::InBase);
        }
    }
}

TEST_F(BoardTest, NewBoardHasNoCapturedPiece)
{
    EXPECT_FALSE(board.GetLastCapturedPiece().has_value());
}


TEST_F(BoardTest, GetPieceWithInvalidIdThrows)
{
    EXPECT_THROW(board.GetPiece({ EColor::Red, -1 }), std::out_of_range);
    EXPECT_THROW(board.GetPiece({ EColor::Red, kPiecesPerPlayer }), std::out_of_range);
    EXPECT_THROW(board.GetPiece({ kInvalidColor, 0 }), std::out_of_range);
}

TEST_F(BoardTest, MoveWithInvalidPieceIdIsRejected)
{
    EXPECT_EQ(board.CheckMove({ EColor::Red, kPiecesPerPlayer }, 6), EMoveResult::InvalidPieceId);
    EXPECT_EQ(board.MovePiece({ kInvalidColor, 0 }, 6), EMoveResult::InvalidPieceId);
}

TEST_F(BoardTest, MoveWithInvalidDiceValueIsRejected)
{
    EXPECT_EQ(board.CheckMove(kRed0, kMinDiceValue - 1), EMoveResult::InvalidDiceValue);
    EXPECT_EQ(board.CheckMove(kRed0, kMaxDiceValue + 1), EMoveResult::InvalidDiceValue);
}

TEST_F(BoardTest, PieceInBaseNeedsSixToLeave)
{
    for (int diceValue = kMinDiceValue; diceValue < kDiceValueToLeaveBase; ++diceValue)
    {
        EXPECT_EQ(board.CheckMove(kRed0, diceValue), EMoveResult::NeedsSixToLeaveBase)
            << "zar: " << diceValue;
    }
}

TEST_F(BoardTest, SixMovesPieceFromBaseToItsStartSquare)
{
    EXPECT_EQ(board.MovePiece(kBlue0, kDiceValueToLeaveBase), EMoveResult::Moved);

    const IPiece& piece = board.GetPiece(kBlue0);
    EXPECT_EQ(piece.GetState(), EPieceState::OnTrack);
    EXPECT_EQ(piece.GetProgress(), kStartProgress);
    EXPECT_EQ(piece.GetTrackSquare(), GetStartSquare(EColor::Blue));
}


TEST_F(BoardTest, PieceOnTrackMovesByDiceValue)
{
    PlaceOnProgress(kRed0, 0);

    EXPECT_EQ(board.MovePiece(kRed0, 4), EMoveResult::Moved);
    EXPECT_EQ(board.GetPiece(kRed0).GetProgress(), 4);
}

TEST_F(BoardTest, PieceMovesPastLastSquareOfTrackToSquareZero)
{
    PlaceOnProgress(kYellow0, 15);

    EXPECT_EQ(board.GetPiece(kYellow0).GetTrackSquare(), 5);
}

TEST_F(BoardTest, PieceCanJumpOverOwnPiece)
{
    PlaceOnProgress(kRed0, 2);
    PlaceOnProgress(kRed1, 0);

    EXPECT_EQ(board.MovePiece(kRed1, 5), EMoveResult::Moved);
    EXPECT_EQ(board.GetPiece(kRed1).GetProgress(), 5);
}

TEST_F(BoardTest, CheckMoveDoesNotChangeBoard)
{
    PlaceOnProgress(kRed0, 3);

    EXPECT_EQ(board.CheckMove(kRed0, 5), EMoveResult::Moved);
    EXPECT_EQ(board.GetPiece(kRed0).GetProgress(), 3);
}

TEST_F(BoardTest, RejectedMoveDoesNotChangeBoard)
{
    EXPECT_EQ(board.MovePiece(kRed0, 3), EMoveResult::NeedsSixToLeaveBase);
    EXPECT_EQ(board.GetPiece(kRed0).GetState(), EPieceState::InBase);
}


TEST_F(BoardTest, CannotLeaveBaseWhenOwnPieceIsOnStartSquare)
{
    PlaceOnProgress(kRed0, 0);

    EXPECT_EQ(board.MovePiece(kRed1, kDiceValueToLeaveBase), EMoveResult::BlockedByOwnPiece);
    EXPECT_EQ(board.GetPiece(kRed1).GetState(), EPieceState::InBase);
}

TEST_F(BoardTest, CannotLandOnOwnPieceOnTrack)
{
    PlaceOnProgress(kRed0, 5);
    PlaceOnProgress(kRed1, 0);

    EXPECT_EQ(board.MovePiece(kRed1, 5), EMoveResult::BlockedByOwnPiece);
    EXPECT_EQ(board.GetPiece(kRed1).GetProgress(), 0);
}

TEST_F(BoardTest, CannotLandOnOwnPieceInHome)
{
    PlaceOnProgress(kRed0, kLastHomeProgress);
    PlaceOnProgress(kRed1, kLastTrackProgress);

    EXPECT_EQ(board.CheckMove(kRed1, 4), EMoveResult::BlockedByOwnPiece);
}


TEST_F(BoardTest, LandingOnOpponentCapturesIt)
{
    PlaceOnProgress(kBlue0, 0);
    PlaceOnProgress(kRed0, 6);

    EXPECT_EQ(board.MovePiece(kRed0, 4), EMoveResult::Captured);

    EXPECT_EQ(board.GetPiece(kRed0).GetTrackSquare(), 10);
    EXPECT_EQ(board.GetPiece(kBlue0).GetState(), EPieceState::InBase);
    ASSERT_TRUE(board.GetLastCapturedPiece().has_value());
    EXPECT_EQ(*board.GetLastCapturedPiece(), kBlue0);
}

TEST_F(BoardTest, LeavingBaseCapturesOpponentOnStartSquare)
{
    PlaceOnProgress(kRed0, GetStartSquare(EColor::Blue));

    EXPECT_EQ(board.MovePiece(kBlue0, kDiceValueToLeaveBase), EMoveResult::Captured);

    EXPECT_EQ(board.GetPiece(kRed0).GetState(), EPieceState::InBase);
    EXPECT_EQ(*board.GetLastCapturedPiece(), kRed0);
}

TEST_F(BoardTest, CheckMoveReportsCaptureWithoutCapturing)
{
    PlaceOnProgress(kBlue0, 0);
    PlaceOnProgress(kRed0, 6);

    EXPECT_EQ(board.CheckMove(kRed0, 4), EMoveResult::Captured);
    EXPECT_EQ(board.GetPiece(kBlue0).GetState(), EPieceState::OnTrack);
}

TEST_F(BoardTest, LastCapturedPieceIsClearedByNextNormalMove)
{
    PlaceOnProgress(kBlue0, 0);
    PlaceOnProgress(kRed0, 6);
    board.MovePiece(kRed0, 4);

    EXPECT_EQ(board.MovePiece(kRed0, 1), EMoveResult::Moved);
    EXPECT_FALSE(board.GetLastCapturedPiece().has_value());
}

TEST_F(BoardTest, PieceInHomeCannotBeCaptured)
{
    PlaceOnProgress(kRed0, kFirstHomeProgress);
    PlaceOnProgress(kBlue0, 30);

    EXPECT_EQ(board.GetPiece(kBlue0).GetTrackSquare(), GetStartSquare(EColor::Red));
    EXPECT_EQ(board.GetPiece(kRed0).GetState(), EPieceState::InHome);
    EXPECT_FALSE(board.GetLastCapturedPiece().has_value());
}


TEST_F(BoardTest, ExactDiceValueEntersHome)
{
    PlaceOnProgress(kRed0, 36);

    EXPECT_EQ(board.MovePiece(kRed0, 4), EMoveResult::Moved);

    EXPECT_EQ(board.GetPiece(kRed0).GetState(), EPieceState::InHome);
    EXPECT_EQ(board.GetPiece(kRed0).GetHomeSquare(), 0);
}

TEST_F(BoardTest, DiceValuePastLastHomeSquareIsRejected)
{
    PlaceOnProgress(kRed0, kLastTrackProgress);

    EXPECT_EQ(board.CheckMove(kRed0, 4), EMoveResult::Moved);
    EXPECT_EQ(board.CheckMove(kRed0, 5), EMoveResult::OvershootsHome);
}

TEST_F(BoardTest, PieceInHomeCanStillMoveForward)
{
    PlaceOnProgress(kRed0, kFirstHomeProgress);

    EXPECT_EQ(board.MovePiece(kRed0, 3), EMoveResult::Moved);
    EXPECT_EQ(board.GetPiece(kRed0).GetHomeSquare(), kHomeLength - 1);
}

TEST_F(BoardTest, PieceOnLastHomeSquareCannotMove)
{
    PlaceOnProgress(kRed0, kLastHomeProgress);

    EXPECT_EQ(board.CheckMove(kRed0, 1), EMoveResult::OvershootsHome);
}


TEST_F(BoardTest, NoMovablePiecesWhenAllInBaseAndNoSix)
{
    EXPECT_TRUE(board.GetMovablePieces(EColor::Red, 3).empty());
}

TEST_F(BoardTest, AllPiecesMovableWhenAllInBaseAndSix)
{
    EXPECT_EQ(board.GetMovablePieces(EColor::Red, 6), (std::vector<int>{ 0, 1, 2, 3 }));
}

TEST_F(BoardTest, PiecesBlockedOnStartSquareAreNotMovable)
{
    PlaceOnProgress(kRed0, 0);

    EXPECT_EQ(board.GetMovablePieces(EColor::Red, 6), (std::vector<int>{ 0 }));
}

TEST_F(BoardTest, NoMovablePiecesForInvalidDiceValue)
{
    EXPECT_TRUE(board.GetMovablePieces(EColor::Red, 0).empty());
}


TEST_F(BoardTest, PlayerHasNotWonAtStart)
{
    EXPECT_FALSE(board.AreAllPiecesHome(EColor::Red));
}

TEST_F(BoardTest, PlayerHasNotWonWithThreePiecesHome)
{
    PlaceOnProgress(kRed0, 43);
    PlaceOnProgress(kRed1, 42);
    PlaceOnProgress(kRed2, 41);

    EXPECT_FALSE(board.AreAllPiecesHome(EColor::Red));
}

TEST_F(BoardTest, PlayerWinsWhenAllPiecesAreHome)
{
    PlaceOnProgress(kRed0, 43);
    PlaceOnProgress(kRed1, 42);
    PlaceOnProgress(kRed2, 41);
    PlaceOnProgress(kRed3, 40);

    EXPECT_TRUE(board.AreAllPiecesHome(EColor::Red));
    EXPECT_FALSE(board.AreAllPiecesHome(EColor::Blue));
}

TEST_F(BoardTest, InvalidColorHasNotWon)
{
    EXPECT_FALSE(board.AreAllPiecesHome(kInvalidColor));
}

TEST_F(BoardTest, ResetSendsAllPiecesToBase)
{
    PlaceOnProgress(kBlue0, 0);
    PlaceOnProgress(kRed0, 6);
    board.MovePiece(kRed0, 4);

    board.Reset();

    EXPECT_EQ(board.GetPiece(kRed0).GetState(), EPieceState::InBase);
    EXPECT_EQ(board.GetPiece(kBlue0).GetState(), EPieceState::InBase);
    EXPECT_FALSE(board.GetLastCapturedPiece().has_value());
}
