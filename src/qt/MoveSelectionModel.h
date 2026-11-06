#pragma once

#include <optional>
#include <vector>

#include "Board.h"
#include "Move.h"
#include "Types.h"

namespace chess {

// Click-select / click-move state machine used by ChessBoardWidget.
// Intentionally Qt-free so selection rules can be unit-tested directly.
class MoveSelectionModel {
public:
    void setPosition(const Board& board);
    void clearSelection();

    // First click selects an own piece (if any). Second click either completes
    // a legal move, re-selects another own piece, or deselects.
    std::optional<Move> handleSquareClick(Square square);

    Square selectedSquare() const;
    const std::vector<Square>& legalDestinations() const;
    bool isLegalDestination(Square square) const;

private:
    void selectSquare(Square square);
    std::optional<Move> findMove(Square from, Square to) const;

    Board board_{};
    std::vector<Move> legalMoves_{};
    Square selected_ = kNoSquare;
    std::vector<Square> legalDestinations_{};
};

} // namespace chess
