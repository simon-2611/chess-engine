#pragma once

#include "Board.h"
#include "Move.h"
#include "Types.h"

namespace chess {

// Abstraction of the presentation layer. GameController only knows this
// interface, so that console output, a graphical user interface or a
// UCI adapter (for connection to existing chess GUIs) are interchangeable.
class UserInterface {
public:
    virtual ~UserInterface() = default;

    virtual void displayBoard(const Board& board) = 0;

    // Blocks until the human to move has entered a valid move.
    virtual Move requestHumanMove(const Board& board) = 0;

    virtual void announceResult(GameResult result) = 0;
    virtual void reportEngineMove(const Move& move, int scoreCentipawns) = 0;
};

} // namespace chess
