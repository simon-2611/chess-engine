#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "Player.h"
#include "SearchEngine.h"

namespace chess {

// Thrown when a SearchEngine returns a move that is not legal in the position.
// Used by the Qt layer to keep invalid engine output away from Board::makeMove
// without changing core Player/GameController abstractions.
class InvalidEngineMove : public std::runtime_error {
public:
    explicit InvalidEngineMove(const std::string& message)
        : std::runtime_error(message) {}
};

// EnginePlayer wrapper that validates bestMove against MoveGenerator::generateLegalMoves.
class ValidatingEnginePlayer : public Player {
public:
    explicit ValidatingEnginePlayer(std::unique_ptr<SearchEngine> searchEngine);

    Move getMove(Board& board, const SearchLimits& limits) override;

private:
    std::unique_ptr<SearchEngine> searchEngine_;
};

} // namespace chess
