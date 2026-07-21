#pragma once

#include <iostream>
#include "UserInterface.h"

namespace chess {

// Simplest UserInterface implementation: text output to stdout,
// input as text via stdin. Serves as a functional placeholder until
// a graphical interface or a UCI adapter exists.
class ConsoleUI : public UserInterface {
public:
    ConsoleUI(std::istream& in = std::cin, std::ostream& out = std::cout);

    void displayBoard(const Board& board) override;
    Move requestHumanMove(const Board& board) override;
    void announceResult(GameResult result) override;
    void reportEngineMove(const Move& move, int scoreCentipawns) override;

private:
    std::istream& in_;
    std::ostream& out_;
};

} // namespace chess
