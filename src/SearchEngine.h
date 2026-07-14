#pragma once

#include <chrono>
#include <memory>

#include "Board.h"
#include "Evaluator.h"
#include "Move.h"
#include "TranspositionTable.h"

namespace chess {

// Result of a search call including diagnostic data, useful e.g. to compare
// different search algorithms (node count, time) with each other.
struct SearchResult {
    Move bestMove;
    int score = 0;
    int depthReached = 0;
    long long nodesSearched = 0;
    std::chrono::milliseconds elapsed{0};
};

// Optional termination criteria, so that e.g. IterativeDeepeningSearch can
// implement a time-budget-controlled search.
struct SearchLimits {
    int maxDepth = 4;
    std::chrono::milliseconds maxTime{0}; // 0 = no time limit
};

// Abstract base class for all search algorithms. The GameController holds only
// a pointer to this interface - which specific algorithm (Minimax, AlphaBeta, ...)
// actually runs remains transparent to it.
class SearchEngine {
public:
    SearchEngine(std::shared_ptr<Evaluator> evaluator,
                 std::shared_ptr<TranspositionTable> transpositionTable = nullptr);
    virtual ~SearchEngine() = default;

    virtual SearchResult findBestMove(Board& board, const SearchLimits& limits) = 0;

protected:
    std::shared_ptr<Evaluator> evaluator_;
    std::shared_ptr<TranspositionTable> transpositionTable_; // may be nullptr
    long long nodesSearched_ = 0;
};

} // namespace chess
