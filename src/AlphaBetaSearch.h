#pragma once

#include "SearchEngine.h"

namespace chess {

// Minimax with Alpha-Beta pruning; optional use of a TranspositionTable
// (if passed to the constructor) and a Quiescence Search at the
// leaf nodes to mitigate the horizon effect in hanging capture sequences.
class AlphaBetaSearch : public SearchEngine {
public:
    using SearchEngine::SearchEngine;

    SearchResult findBestMove(Board& board, const SearchLimits& limits) override;

private:
    int alphaBeta(Board& board, int depth, int alpha, int beta, Color maximizingColor);
    int quiescence(Board& board, int alpha, int beta, Color maximizingColor);
};

} // namespace chess
