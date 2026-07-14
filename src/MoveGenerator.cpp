#include "MoveGenerator.h"

namespace chess {

std::vector<Move> MoveGenerator::generatePseudoLegalMoves(const Board& /*board*/) {
    // TODO: generate target squares per piece type according to movement rules.
    return {};
}

std::vector<Move> MoveGenerator::generateLegalMoves(const Board& board) {
    // TODO: filter generatePseudoLegalMoves() by tentatively executing each move
    // and checking board.isInCheck(sideToMove).
    (void)board;
    return {};
}

std::vector<Move> MoveGenerator::generateCaptureMoves(const Board& /*board*/) {
    // TODO: like generateLegalMoves(), but only moves with isCapture() == true.
    return {};
}

bool MoveGenerator::isMoveLegal(const Board& board, const Move& move) {
    // TODO: check if 'move' is contained in generateLegalMoves(board).
    (void)board;
    (void)move;
    return false;
}

bool MoveGenerator::hasAnyLegalMove(const Board& board) {
    return !generateLegalMoves(board).empty();
}

} // namespace chess
