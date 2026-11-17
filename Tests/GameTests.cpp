#include "pch.h"

#include "BoardConstants.h"
#include "Game.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <stdexcept>
#include <tuple>
#include <vector>

using namespace BoardConstants;

namespace
{
    constexpr int kDiceValueWithoutExtraTurn = 3;
    constexpr int kDiceValueForExtraTurn = 6;

    // În joc, rândul se încheie după aruncare și mutare; aici îl încheiem direct.
    class TestableGame : public Game
    {
    public:
        using Game::EndTurn;
    };
}

class GameTest : public ::testing::Test
{
protected:
    TestableGame game;
};


TEST_F(GameTest, NewGameIsNotStarted)
{
    EXPECT_EQ(game.GetState(), EGameState::NotStarted);
    EXPECT_TRUE(game.GetPlayers().empty());
}

TEST_F(GameTest, CurrentPlayerBeforeStartThrows)
{
    EXPECT_THROW(game.GetCurrentPlayer(), std::logic_error);
}

TEST_F(GameTest, EndTurnBeforeStartThrows)
{
    EXPECT_THROW(game.EndTurn(kDiceValueWithoutExtraTurn), std::logic_error);
}


TEST_F(GameTest, StartWaitsForFirstPlayerToRoll)
{
    game.Start(IGame::kMaxPlayerCount);

    EXPECT_EQ(game.GetState(), EGameState::WaitingForRoll);
    EXPECT_EQ(game.GetCurrentPlayer(), EColor::Red);
}

TEST_F(GameTest, StartWithInvalidPlayerCountKeepsCurrentGame)
{
    game.Start(IGame::kMaxPlayerCount);
    game.EndTurn(kDiceValueWithoutExtraTurn);

    EXPECT_THROW(game.Start(IGame::kMaxPlayerCount + 1), std::invalid_argument);

    EXPECT_EQ(game.GetPlayers().size(), static_cast<std::size_t>(IGame::kMaxPlayerCount));
    EXPECT_EQ(game.GetCurrentPlayer(), EColor::Blue);
}

TEST_F(GameTest, RestartGivesTurnBackToFirstPlayer)
{
    game.Start(IGame::kMaxPlayerCount);
    game.EndTurn(kDiceValueWithoutExtraTurn);

    game.Start(IGame::kMinPlayerCount);

    EXPECT_EQ(game.GetPlayers().size(), static_cast<std::size_t>(IGame::kMinPlayerCount));
    EXPECT_EQ(game.GetCurrentPlayer(), EColor::Red);
    EXPECT_EQ(game.GetState(), EGameState::WaitingForRoll);
}


TEST_F(GameTest, GetPieceReturnsPieceFromBoard)
{
    game.Start(IGame::kMaxPlayerCount);

    const IPiece& piece = game.GetPiece({ EColor::Green, 2 });

    EXPECT_EQ(piece.GetColor(), EColor::Green);
    EXPECT_EQ(piece.GetIndex(), 2);
    EXPECT_EQ(piece.GetState(), EPieceState::InBase);
}

TEST_F(GameTest, GetPieceWithInvalidIdThrows)
{
    EXPECT_THROW(game.GetPiece({ EColor::Red, kPiecesPerPlayer }), std::out_of_range);
}


TEST_F(GameTest, TurnPassesToNextPlayerWithoutSix)
{
    game.Start(IGame::kMaxPlayerCount);

    game.EndTurn(kDiceValueWithoutExtraTurn);

    EXPECT_EQ(game.GetCurrentPlayer(), EColor::Blue);
}

TEST_F(GameTest, SixGivesSamePlayerAnotherTurn)
{
    game.Start(IGame::kMaxPlayerCount);

    game.EndTurn(kDiceValueForExtraTurn);

    EXPECT_EQ(game.GetCurrentPlayer(), EColor::Red);
}

TEST_F(GameTest, TurnPassesAfterExtraTurnEnds)
{
    game.Start(IGame::kMaxPlayerCount);

    game.EndTurn(kDiceValueForExtraTurn);
    game.EndTurn(kDiceValueWithoutExtraTurn);

    EXPECT_EQ(game.GetCurrentPlayer(), EColor::Blue);
}

TEST_F(GameTest, TurnsFollowPlayerOrderAndWrapAround)
{
    game.Start(3);

    for (const EColor expectedPlayer : { EColor::Blue, EColor::Green, EColor::Red })
    {
        game.EndTurn(kDiceValueWithoutExtraTurn);
        EXPECT_EQ(game.GetCurrentPlayer(), expectedPlayer);
    }
}

TEST_F(GameTest, NextPlayerStartsByRolling)
{
    game.Start(IGame::kMaxPlayerCount);

    game.EndTurn(kDiceValueWithoutExtraTurn);

    EXPECT_EQ(game.GetState(), EGameState::WaitingForRoll);
}


class GameStartTest : public ::testing::TestWithParam<std::tuple<int, std::vector<EColor>>>
{
};

TEST_P(GameStartTest, PlayersGetColorsInOrder)
{
    const auto [playerCount, expectedPlayers] = GetParam();
    Game game;

    game.Start(playerCount);

    EXPECT_EQ(game.GetPlayers(), expectedPlayers);
}

INSTANTIATE_TEST_CASE_P(
    ValidPlayerCounts,
    GameStartTest,
    ::testing::Values(
        std::make_tuple(2, std::vector<EColor>{ EColor::Red, EColor::Blue }),
        std::make_tuple(3, std::vector<EColor>{ EColor::Red, EColor::Blue, EColor::Green }),
        std::make_tuple(4, std::vector<EColor>{ EColor::Red, EColor::Blue, EColor::Green, EColor::Yellow })));


class GameInvalidPlayerCountTest : public ::testing::TestWithParam<int>
{
};

TEST_P(GameInvalidPlayerCountTest, StartThrowsAndGameStaysNotStarted)
{
    Game game;

    EXPECT_THROW(game.Start(GetParam()), std::invalid_argument);
    EXPECT_EQ(game.GetState(), EGameState::NotStarted);
}

INSTANTIATE_TEST_CASE_P(
    OutOfRange,
    GameInvalidPlayerCountTest,
    ::testing::Values(0, IGame::kMinPlayerCount - 1, IGame::kMaxPlayerCount + 1));
