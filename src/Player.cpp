#include "Player.h"

#include <utility>

namespace chess {

HumanPlayer::HumanPlayer(std::shared_ptr<UserInterface> ui) : ui_(std::move(ui)) {}

Move HumanPlayer::getMove(Board& board, const SearchLimits& /*limits*/) {
    return ui_->requestHumanMove(board);
}

EnginePlayer::EnginePlayer(std::unique_ptr<SearchEngine> searchEngine)
    : searchEngine_(std::move(searchEngine)) {}

Move EnginePlayer::getMove(Board& board, const SearchLimits& limits) {
    SearchResult result = searchEngine_->findBestMove(board, limits);
    return result.bestMove;
}

} // namespace chess
