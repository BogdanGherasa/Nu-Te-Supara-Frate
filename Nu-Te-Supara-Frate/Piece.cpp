#include "Piece.h"

#include "BoardConstants.h"

#include <stdexcept>

using namespace BoardConstants;

Piece::Piece(EColor color, int index)
    : m_color(color)
    , m_index(index)
    , m_progress(kBaseProgress)
{
}

EColor Piece::GetColor() const
{
    return m_color;
}

int Piece::GetIndex() const
{
    return m_index;
}

EPieceState Piece::GetState() const
{
    if (m_progress == kBaseProgress)
    {
        return EPieceState::InBase;
    }
    if (m_progress >= kFirstHomeProgress)
    {
        return EPieceState::InHome;
    }
    return EPieceState::OnTrack;
}

int Piece::GetProgress() const
{
    return m_progress;
}

int Piece::GetTrackSquare() const
{
    return ProgressToTrackSquare(m_color, m_progress);
}

int Piece::GetHomeSquare() const
{
    return ProgressToHomeSquare(m_progress);
}

void Piece::SetProgress(int progress)
{
    if (progress < kBaseProgress || progress > kLastHomeProgress)
    {
        throw std::out_of_range("Piece::SetProgress: progres în afara tablei");
    }
    m_progress = progress;
}

void Piece::ReturnToBase()
{
    m_progress = kBaseProgress;
}
