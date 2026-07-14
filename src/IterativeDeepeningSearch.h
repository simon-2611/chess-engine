#pragma once

#include <memory>

#include "SearchEngine.h"

namespace chess {

// Clear example of the combinability of the strategy pattern: instead of
// implementing its own search logic, this class wraps another SearchEngine
// (typically AlphaBetaSearch) and calls it with increasing depth until the
// time budget from SearchLimits is exhausted. This returns a valid
// "best so far" move at any time, even if the search must be aborted early.
class IterativeDeepeningSearch : public SearchEngine {
public:
    IterativeDeepeningSearch(std::shared_ptr<Evaluator> evaluator,
                              std::unique_ptr<SearchEngine> innerEngine,
                              std::shared_ptr<TranspositionTable> transpositionTable = nullptr);

    SearchResult findBestMove(Board& board, const SearchLimits& limits) override;

private:
    std::unique_ptr<SearchEngine> innerEngine_;
};

} // namespace chess
