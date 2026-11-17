#include "pch.h"

#include "BoardConstants.h"
#include "Piece.h"

#include <gtest/gtest.h>

#include <stdexcept>
#include <tuple>

using namespace BoardConstants;

TEST(PieceTest, NewPieceIsInBase)
{
    const Piece piece(EColor::Blue, 2);

    EXPECT_EQ(piece.GetColor(), EColor::Blue);
    EXPECT_EQ(piece.GetIndex(), 2);
    EXPECT_EQ(piece.GetState(), EPieceState::InBase);
    EXPECT_EQ(piece.GetProgress(), kBaseProgress);
    EXPECT_EQ(piece.GetTrackSquare(), kNoSquare);
    EXPECT_EQ(piece.GetHomeSquare(), kNoSquare);
}

TEST(PieceTest, ProgressOnTrackPutsPieceOnTrack)
{
    Piece piece(EColor::Red, 0);

    piece.SetProgress(17);

    EXPECT_EQ(piece.GetState(), EPieceState::OnTrack);
    EXPECT_EQ(piece.GetProgress(), 17);
    EXPECT_EQ(piece.GetTrackSquare(), 17);
    EXPECT_EQ(piece.GetHomeSquare(), kNoSquare);
}

TEST(PieceTest, TrackSquareWrapsAroundAfterLastSquare)
{
    Piece piece(EColor::Yellow, 0);

    piece.SetProgress(15);

    EXPECT_EQ(piece.GetTrackSquare(), 5);
}

TEST(PieceTest, LastTrackProgressIsSquareBeforeOwnStart)
{
    Piece piece(EColor::Blue, 0);

    piece.SetProgress(kLastTrackProgress);

    EXPECT_EQ(piece.GetState(), EPieceState::OnTrack);
    EXPECT_EQ(piece.GetTrackSquare(), GetStartSquare(EColor::Blue) - 1);
}

TEST(PieceTest, FirstHomeProgressPutsPieceInFirstHomeSquare)
{
    Piece piece(EColor::Green, 1);

    piece.SetProgress(kFirstHomeProgress);

    EXPECT_EQ(piece.GetState(), EPieceState::InHome);
    EXPECT_EQ(piece.GetHomeSquare(), 0);
    EXPECT_EQ(piece.GetTrackSquare(), kNoSquare);
}

TEST(PieceTest, LastHomeProgressPutsPieceInLastHomeSquare)
{
    Piece piece(EColor::Green, 1);

    piece.SetProgress(kLastHomeProgress);

    EXPECT_EQ(piece.GetState(), EPieceState::InHome);
    EXPECT_EQ(piece.GetHomeSquare(), kHomeLength - 1);
}

TEST(PieceTest, SetProgressBelowBaseThrows)
{
    Piece piece(EColor::Red, 0);

    EXPECT_THROW(piece.SetProgress(kBaseProgress - 1), std::out_of_range);
    EXPECT_EQ(piece.GetProgress(), kBaseProgress);
}

TEST(PieceTest, SetProgressPastLastHomeSquareThrows)
{
    Piece piece(EColor::Red, 0);

    EXPECT_THROW(piece.SetProgress(kLastHomeProgress + 1), std::out_of_range);
    EXPECT_EQ(piece.GetProgress(), kBaseProgress);
}

TEST(PieceTest, ReturnToBaseSendsPieceBackToBase)
{
    Piece piece(EColor::Red, 0);
    piece.SetProgress(25);

    piece.ReturnToBase();

    EXPECT_EQ(piece.GetState(), EPieceState::InBase);
    EXPECT_EQ(piece.GetProgress(), kBaseProgress);
}

class PieceStartSquareTest : public ::testing::TestWithParam<std::tuple<EColor, int>>
{
};

TEST_P(PieceStartSquareTest, StartProgressIsOnColorStartSquare)
{
    const auto [color, expectedStartSquare] = GetParam();
    Piece piece(color, 0);

    piece.SetProgress(kStartProgress);

    EXPECT_EQ(piece.GetTrackSquare(), expectedStartSquare);
}

INSTANTIATE_TEST_CASE_P(
    AllColors,
    PieceStartSquareTest,
    ::testing::Values(
        std::make_tuple(EColor::Red, 0),
        std::make_tuple(EColor::Blue, 10),
        std::make_tuple(EColor::Green, 20),
        std::make_tuple(EColor::Yellow, 30)));
