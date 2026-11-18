#pragma once

#include "Board.h"
#include "IGame.h"

#include <cstddef>
#include <vector>

class Game : public IGame
{
public:

    void Start(int playerCount) override;
    EGameState GetState() const override;
    EColor GetCurrentPlayer() const override;
    std::vector<EColor> GetPlayers() const override;
    const IPiece& GetPiece(PieceId id) const override;

protected:
    void EndTurn(int diceValue);

private:
    static bool IsValidPlayerCount(int playerCount);

    bool IsInProgress() const;
    void ThrowIfNotStarted() const;
    void PassTurnToNextPlayer();

    Board m_board;
    std::vector<EColor> m_players;
    std::size_t m_currentPlayerIndex = 0;
    EGameState m_state = EGameState::NotStarted;
};
