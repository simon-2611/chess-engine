#include "ConsoleUI.h"

#include <iostream>
#include <string>

namespace chess {

void ConsoleUI::displayBoard(const Board& /*board*/) {
    // TODO: Output board as 8x8 text grid (e.g., using pieceAt()).
    std::cout << "[Board output follows]\n";
}

Move ConsoleUI::requestHumanMove(const Board& /*board*/) {
    std::cout << "Enter move (e.g. e2e4): ";
    std::string input;
    std::getline(std::cin, input);
    // TODO: Parse 'input' (square names -> Square) and convert into a Move
    // validated via MoveGenerator.
    return Move{};
}

void ConsoleUI::announceResult(GameResult result) {
    switch (result) {
        case GameResult::WhiteWinsByCheckmate:
            std::cout << "White wins by checkmate.\n";
            break;
        case GameResult::BlackWinsByCheckmate:
            std::cout << "Black wins by checkmate.\n";
            break;
        case GameResult::DrawByStalemate:
            std::cout << "Draw by stalemate.\n";
            break;
        case GameResult::DrawByFiftyMoveRule:
            std::cout << "Draw by 50-move rule.\n";
            break;
        case GameResult::DrawByRepetition:
            std::cout << "Draw by repetition.\n";
            break;
        case GameResult::DrawByInsufficientMaterial:
            std::cout << "Draw by insufficient material.\n";
            break;
        case GameResult::Ongoing:
        default:
            break;
    }
}

void ConsoleUI::reportEngineMove(const Move& /*move*/, int scoreCentipawns) {
    std::cout << "Engine plays a move (evaluation: " << scoreCentipawns << " cp)\n";
}

} // namespace chess
