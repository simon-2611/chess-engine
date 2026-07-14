#pragma once

#include "SearchEngine.h"

namespace chess {

// Pure Minimax without alpha-beta pruning. Deliberately kept as its own
// simple implementation - e.g. to test AlphaBetaSearch later against this
// reference for correctness (same result, fewer nodes).
class MinimaxSearch : public SearchEngine {
public:
    using SearchEngine::SearchEngine;

    SearchResult findBestMove(Board& board, const SearchLimits& limits) override;

private:
    int minimax(Board& board, int depth, Color maximizingColor);
};

} // namespace chess
