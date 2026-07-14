#include "MinimaxSearch.h"

#include "MoveGenerator.h"

namespace chess {

int MinimaxSearch::minimax(Board& board, int depth, Color maximizingColor) {
    // TODO: terminate if depth == 0 -> evaluator_->evaluate(board).
    // Otherwise: go through all legal moves, board.makeMove()/undoMove(),
    // recursively call minimax(depth - 1, ...) and choose the best result
    // depending on maximizing/minimizing side.
    (void)board;
    (void)depth;
    (void)maximizingColor;
    return evaluator_->evaluate(board);
}

SearchResult MinimaxSearch::findBestMove(Board& board, const SearchLimits& limits) {
    // TODO: try out generateLegalMoves(board), call minimax(),
    // return best move in SearchResult, increment nodesSearched_.
    (void)MoveGenerator::generateLegalMoves(board);
    (void)limits;

    SearchResult result;
    result.score = evaluator_->evaluate(board);
    return result;
}

} // namespace chess
