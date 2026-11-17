#include "Game.h"

#include "BoardConstants.h"

#include <stdexcept>

namespace
{
    constexpr int kDiceValueForExtraTurn = BoardConstants::kMaxDiceValue;
}

void Game::Start(int playerCount)
{
    if (!IsValidPlayerCount(playerCount))
    {
        throw std::invalid_argument("Game: numar de jucatori invalid");
    }

    m_board.Reset();
    m_players.clear();
    for (int colorIndex = 0; colorIndex < playerCount; ++colorIndex)
    {
        m_players.push_back(static_cast<EColor>(colorIndex));
    }
    m_currentPlayerIndex = 0;
    m_state = EGameState::WaitingForRoll;
}

EGameState Game::GetState() const
{
    return m_state;
}

EColor Game::GetCurrentPlayer() const
{
    ThrowIfNotStarted();
    return m_players[m_currentPlayerIndex];
}

std::vector<EColor> Game::GetPlayers() const
{
    return m_players;
}

const IPiece& Game::GetPiece(PieceId id) const
{
    return m_board.GetPiece(id);
}

void Game::EndTurn(int diceValue)
{
    if (!IsInProgress())
    {
        throw std::logic_error("Game: jocul nu este in desfasurare");
    }

    if (diceValue != kDiceValueForExtraTurn)
    {
        PassTurnToNextPlayer();
    }
    m_state = EGameState::WaitingForRoll;
}

bool Game::IsValidPlayerCount(int playerCount)
{
    return playerCount >= kMinPlayerCount && playerCount <= kMaxPlayerCount;
}

bool Game::IsInProgress() const
{
    return m_state == EGameState::WaitingForRoll || m_state == EGameState::WaitingForMove;
}

void Game::ThrowIfNotStarted() const
{
    if (m_state == EGameState::NotStarted)
    {
        throw std::logic_error("Game: jocul nu a inceput");
    }
}

void Game::PassTurnToNextPlayer()
{
    m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
}
