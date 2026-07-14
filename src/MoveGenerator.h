#pragma once

#include <vector>

#include "Board.h"
#include "Move.h"

namespace chess {

// Stateless move generation, intentionally as a pure function interface
// (no instance data), so that it can be used by both Board/GameController
// and directly by a SearchEngine.
class MoveGenerator {
public:
    MoveGenerator() = delete;

    // Generates all pseudo-legal moves (rules of piece movement, but without
    // checking if the own king is in check afterwards). Faster than
    // generateLegalMoves(), useful e.g. for attack detection.
    static std::vector<Move> generatePseudoLegalMoves(const Board& board);

    // Filters generatePseudoLegalMoves() for all moves that would leave
    // the own king in check.
    static std::vector<Move> generateLegalMoves(const Board& board);

    // Only capture moves or moves with check - for quiescence search.
    static std::vector<Move> generateCaptureMoves(const Board& board);

    static bool isMoveLegal(const Board& board, const Move& move);
    static bool hasAnyLegalMove(const Board& board);
};

} // namespace chess
