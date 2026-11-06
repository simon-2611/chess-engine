#include "ValidatingEnginePlayer.h"

#include <algorithm>
#include <utility>

#include "MoveGenerator.h"

namespace chess {

ValidatingEnginePlayer::ValidatingEnginePlayer(std::unique_ptr<SearchEngine> searchEngine)
    : searchEngine_(std::move(searchEngine)) {}

Move ValidatingEnginePlayer::getMove(Board& board, const SearchLimits& limits) {
    if (!searchEngine_) {
        throw InvalidEngineMove("ValidatingEnginePlayer has no search engine");
    }

    const SearchResult result = searchEngine_->findBestMove(board, limits);
    const auto legal = MoveGenerator::generateLegalMoves(board);
    const auto it = std::find(legal.begin(), legal.end(), result.bestMove);
    if (it == legal.end()) {
        throw InvalidEngineMove(
            "SearchEngine returned a move that is not legal in the current position "
            "(stub/unset bestMove is treated as illegal)");
    }
    return *it;
}

} // namespace chess
