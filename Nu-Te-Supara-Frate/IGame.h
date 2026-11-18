#pragma once

#include "BoardConstants.h"
#include "EColor.h"
#include "EGameState.h"
#include "IPiece.h"
#include "PieceId.h"

#include <vector>


class IGame
{
public:
    static constexpr int kMinPlayerCount = 2;
    static constexpr int kMaxPlayerCount = BoardConstants::kPlayerCount;

    virtual ~IGame() = default;

    virtual void Start(int playerCount) = 0;

    virtual EGameState GetState() const = 0;

    virtual EColor GetCurrentPlayer() const = 0;

    virtual std::vector<EColor> GetPlayers() const = 0;

    virtual const IPiece& GetPiece(PieceId id) const = 0;
};
