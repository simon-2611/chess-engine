#include "PositionalEvaluator.h"

namespace chess {

int PositionalEvaluator::materialScore(const Board& /*board*/) const {
    // TODO: analogous to MaterialEvaluator, possibly reuse.
    return 0;
}

int PositionalEvaluator::pieceSquareScore(const Board& /*board*/) const {
    // TODO: add a bonus/malus per piece depending on the square using
    // pre-defined piece-square tables.
    return 0;
}

int PositionalEvaluator::kingSafetyScore(const Board& /*board*/) const {
    // TODO: e.g. pawn shield in front of the king, open files near the king.
    return 0;
}

int PositionalEvaluator::mobilityScore(const Board& /*board*/) const {
    // TODO: use the number of legal moves per color as a measure of mobility.
    return 0;
}

int PositionalEvaluator::evaluate(const Board& board) const {
    return materialScore(board) + pieceSquareScore(board) +
           kingSafetyScore(board) + mobilityScore(board);
}

std::string PositionalEvaluator::name() const {
    return "PositionalEvaluator";
}

} // namespace chess
