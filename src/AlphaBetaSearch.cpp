#include "AlphaBetaSearch.h"

#include "MoveGenerator.h"

namespace chess {

int AlphaBetaSearch::quiescence(Board& board, int alpha, int beta, Color maximizingColor) {
    // TODO: only search capture moves until a "quiet" position is reached
    // to avoid the horizon effect.
    (void)alpha;
    (void)beta;
    (void)maximizingColor;
    return evaluator_->evaluate(board);
}

int AlphaBetaSearch::alphaBeta(Board& board, int depth, int alpha, int beta, Color maximizingColor) {
    // TODO: classic alpha-beta pruning including optional
    // TranspositionTable lookup/store (transpositionTable_, if != nullptr).
    (void)depth;
    (void)alpha;
    (void)beta;
    (void)maximizingColor;
    return evaluator_->evaluate(board);
}

SearchResult AlphaBetaSearch::findBestMove(Board& board, const SearchLimits& limits) {
    // TODO: try out generateLegalMoves(board), call alphaBeta(),
    // choose the best move; move ordering (e.g. captures first) significantly
    // improves pruning efficiency.
    (void)MoveGenerator::generateLegalMoves(board);
    (void)limits;

    SearchResult result;
    result.score = evaluator_->evaluate(board);
    return result;
}

} // namespace chess
