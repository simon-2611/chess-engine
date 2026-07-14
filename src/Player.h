#pragma once

#include <memory>

#include "Board.h"
#include "Move.h"
#include "SearchEngine.h"
#include "UserInterface.h"

namespace chess {

// Abstraction of "who" provides the next move. Enables
// human-vs-human, human-vs-engine and engine-vs-engine with
// the same GameController logic.
class Player {
public:
    virtual ~Player() = default;
    virtual Move getMove(Board& board, const SearchLimits& limits) = 0;
};

class HumanPlayer : public Player {
public:
    explicit HumanPlayer(std::shared_ptr<UserInterface> ui);
    Move getMove(Board& board, const SearchLimits& limits) override;

private:
    std::shared_ptr<UserInterface> ui_;
};

class EnginePlayer : public Player {
public:
    explicit EnginePlayer(std::unique_ptr<SearchEngine> searchEngine);
    Move getMove(Board& board, const SearchLimits& limits) override;

private:
    std::unique_ptr<SearchEngine> searchEngine_;
};

} // namespace chess
