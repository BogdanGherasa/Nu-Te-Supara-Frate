#pragma once

#include "IPiece.h"

class Piece : public IPiece
{
public:
    Piece(EColor color, int index);

    // IPiece
    EColor GetColor() const override;
    int GetIndex() const override;
    EPieceState GetState() const override;
    int GetProgress() const override;
    int GetTrackSquare() const override;
    int GetHomeSquare() const override;

    void SetProgress(int progress);

    void ReturnToBase();

private:
    EColor m_color;
    int m_index;
    int m_progress;
};
