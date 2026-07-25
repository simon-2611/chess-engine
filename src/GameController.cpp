#include "GameController.h"

#include <utility>

#include "MoveGenerator.h"

namespace chess {

GameController::GameController(std::shared_ptr<UserInterface> ui) : ui_(std::move(ui)) {
    newGame();
}

void GameController::newGame() {
    board_ = Board::startingPosition();
    history_.clear();
    result_ = GameResult::Ongoing;
}

void GameController::loadFromFEN(const std::string& fen) {
    board_ = Board::fromFEN(fen);
    history_.clear();
    result_ = GameResult::Ongoing;
}

void GameController::setWhitePlayer(std::unique_ptr<Player> player) {
    whitePlayer_ = std::move(player);
}

void GameController::setBlackPlayer(std::unique_ptr<Player> player) {
    blackPlayer_ = std::move(player);
}

Player& GameController::currentPlayer() {
    return (board_.sideToMove() == Color::White) ? *whitePlayer_ : *blackPlayer_;
}

void GameController::playNextHalfMove(const SearchLimits& limits) {
    if (result_ != GameResult::Ongoing) {
        return;
    }

    Move move = currentPlayer().getMove(board_, limits);

    uint64_t hashBeforeMove = board_.zobristHash();
    UndoInfo undo = board_.makeMove(move);
    history_.push_back(HistoryEntry{move, undo, hashBeforeMove});

    bool hasLegalMoves = MoveGenerator::hasAnyLegalMove(board_);
    
    std::vector<uint64_t> hashes;
    for (const auto& entry : history_) {
        hashes.push_back(entry.hashBeforeMove);
    }
    result_ = board_.evaluateGameResult(hasLegalMoves, hashes);

    if (ui_) {
        ui_->displayBoard(board_);
        if (result_ != GameResult::Ongoing) {
            ui_->announceResult(result_);
        }
    }
}

void GameController::runUntilGameOver(const SearchLimits& limits, int maxHalfMoves) {
    // Parsing positions where the game is already over should also be possible
    bool hasLegalMoves = MoveGenerator::hasAnyLegalMove(board_);
    result_ = board_.evaluateGameResult(hasLegalMoves);
    if (ui_) {
        ui_->displayBoard(board_);
        if (result_ != GameResult::Ongoing) {
            ui_->announceResult(result_);
        }
    }

    for (int i = 0; i < maxHalfMoves && result_ == GameResult::Ongoing; ++i) {
        playNextHalfMove(limits);
    }
}

bool GameController::undoLastHalfMove() {
    if (history_.empty()) {
        return false;
    }
    const HistoryEntry& last = history_.back();
    board_.undoMove(last.move, last.undoInfo);
    history_.pop_back();
    result_ = GameResult::Ongoing;
    return true;
}

const Board& GameController::board() const { return board_; }
GameResult GameController::currentResult() const { return result_; }
const std::vector<HistoryEntry>& GameController::history() const { return history_; }

std::string GameController::exportPGN() const {
    // TODO: Serialize history_ into PGN notation (move pairs, result tag if applicable).
    return "";
}

} // namespace chess
