#include "SearchEngine.h"

#include <utility>

namespace chess {

SearchEngine::SearchEngine(std::shared_ptr<Evaluator> evaluator,
                            std::shared_ptr<TranspositionTable> transpositionTable)
    : evaluator_(std::move(evaluator)), transpositionTable_(std::move(transpositionTable)) {}

} // namespace chess
