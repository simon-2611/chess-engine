#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Board.h"
#include "Move.h"
#include "Player.h"
#include "Types.h"
#include "UserInterface.h"

namespace chess {

// A game history entry: the move, plus the state required for
// undoing it, as well as the Zobrist hash before the move (for position repetition).
struct HistoryEntry {
    Move move;
    UndoInfo undoInfo;
    uint64_t hashBeforeMove = 0;
};

// Central orchestration class. It is unaware of the details of
// move generation or search algorithms—it delegates these to Board/MoveGenerator
// or to the respective Player implementations.
class GameController {
public:
    explicit GameController(std::shared_ptr<UserInterface> ui);

    void newGame();
    void loadFromFEN(const std::string& fen);

    void setWhitePlayer(std::unique_ptr<Player> player);
    void setBlackPlayer(std::unique_ptr<Player> player);

    // Executes exactly one half-move of the side currently to move
    // (queries either a human or an engine search, depending on the Player type)
    // and updates the history as well as the game result.
    void playNextHalfMove(const SearchLimits& limits);

    // Plays the game until it ends (checkmate/stalemate/draw) or
    // until maxHalfMoves is reached—useful for automated engine-vs-engine tests.
    void runUntilGameOver(const SearchLimits& limits, int maxHalfMoves = 500);

    bool undoLastHalfMove();

    const Board& board() const;
    GameResult currentResult() const;
    const std::vector<HistoryEntry>& history() const;

    std::string exportPGN() const;

private:
    Board board_;
    std::shared_ptr<UserInterface> ui_;
    std::unique_ptr<Player> whitePlayer_;
    std::unique_ptr<Player> blackPlayer_;
    std::vector<HistoryEntry> history_;
    GameResult result_ = GameResult::Ongoing;

    Player& currentPlayer();
};

} // namespace chess
