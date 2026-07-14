#include "IterativeDeepeningSearch.h"

#include <utility>

namespace chess {

IterativeDeepeningSearch::IterativeDeepeningSearch(
    std::shared_ptr<Evaluator> evaluator,
    std::unique_ptr<SearchEngine> innerEngine,
    std::shared_ptr<TranspositionTable> transpositionTable)
    : SearchEngine(std::move(evaluator), std::move(transpositionTable)),
      innerEngine_(std::move(innerEngine)) {}

SearchResult IterativeDeepeningSearch::findBestMove(Board& board, const SearchLimits& limits) {
    SearchResult best;
    for (int depth = 1; depth <= limits.maxDepth; ++depth) {
        SearchLimits depthLimits = limits;
        depthLimits.maxDepth = depth;

        best = innerEngine_->findBestMove(board, depthLimits);
        best.depthReached = depth;

        // TODO: check elapsed time against limits.maxTime and exit the loop
        // early if exceeded (current 'best' move remains valid).
    }
    return best;
}

} // namespace chess
