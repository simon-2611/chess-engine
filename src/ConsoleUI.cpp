#include "ConsoleUI.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "MoveGenerator.h"
#include "Types.h"

namespace chess {

ConsoleUI::ConsoleUI(std::istream& in, std::ostream& out) : in_(in), out_(out) {}

void ConsoleUI::displayBoard(const Board& board) {
    out_ << "  a b c d e f g h\n";
    for (int rank = 7; rank >= 0; --rank) {
        out_ << rank + 1 << " ";
        for (int file = 0; file < 8; ++file) {
            Square sq = rank * 8 + file;
            Piece p = board.pieceAt(sq);
            char c = '.';
            if (!p.isEmpty()) {
                switch (p.type) {
                    case PieceType::Pawn:   c = 'p'; break;
                    case PieceType::Knight: c = 'n'; break;
                    case PieceType::Bishop: c = 'b'; break;
                    case PieceType::Rook:   c = 'r'; break;
                    case PieceType::Queen:  c = 'q'; break;
                    case PieceType::King:   c = 'k'; break;
                    default: break;
                }
                if (p.color == Color::White) {
                    c = std::toupper(c);
                }
            }
            out_ << c << (file == 7 ? "" : " ");
        }
        out_ << " " << rank + 1 << "\n";
    }
    out_ << "  a b c d e f g h\n";
    out_ << "FEN: " << board.toFEN() << "\n";
}

Move ConsoleUI::requestHumanMove(const Board& board) {
    std::vector<Move> legalMoves = MoveGenerator::generateLegalMoves(board);

    while (true) {
        out_ << (board.sideToMove() == Color::White ? "White" : "Black") << "'s turn! Enter move (e.g. e2e4): ";
        std::string input;
        if (!std::getline(in_, input) || input.empty()) {
            continue;
        }

        if (input.length() < 4) {
            out_ << "Invalid move format. Use e.g. e2e4\n";
            continue;
        }

        int fromFile = input[0] - 'a';
        int fromRank = input[1] - '1';
        int toFile = input[2] - 'a';
        int toRank = input[3] - '1';

        if (fromFile < 0 || fromFile > 7 || fromRank < 0 || fromRank > 7 ||
            toFile < 0 || toFile > 7 || toRank < 0 || toRank > 7) {
            out_ << "Invalid move: squares out of bounds.\n";
            continue;
        }

        Square from = fromRank * 8 + fromFile;
        Square to = toRank * 8 + toFile;

        MoveFlag flag = MoveFlag::Normal;
        if (input.length() >= 5) {
            char p = std::tolower(input[4]);
            if (p == 'q') flag = MoveFlag::PromotionToQueen;
            else if (p == 'r') flag = MoveFlag::PromotionToRook;
            else if (p == 'b') flag = MoveFlag::PromotionToBishop;
            else if (p == 'n') flag = MoveFlag::PromotionToKnight;
        }

        // Search for the move in legal moves.
        // We need to match from, to, and promotion flag if applicable.
        for (const auto& m : legalMoves) {
            if (m.from() == from && m.to() == to) {
                // If it's a promotion, we need to match the flag.
                if (m.isPromotion()) {
                    if (m.flag() == flag) {
                        return m;
                    }
                } else {
                    return m;
                }
            }
        }

        out_ << "Invalid move: " << input << "\n";
    }
}

void ConsoleUI::announceResult(GameResult result) {
    switch (result) {
        case GameResult::WhiteWinsByCheckmate:
            out_ << "White wins by checkmate.\n";
            break;
        case GameResult::BlackWinsByCheckmate:
            out_ << "Black wins by checkmate.\n";
            break;
        case GameResult::DrawByStalemate:
            out_ << "Draw by stalemate.\n";
            break;
        case GameResult::DrawByFiftyMoveRule:
            out_ << "Draw by 50-move rule.\n";
            break;
        case GameResult::DrawByRepetition:
            out_ << "Draw by repetition.\n";
            break;
        case GameResult::DrawByInsufficientMaterial:
            out_ << "Draw by insufficient material.\n";
            break;
        case GameResult::Ongoing:
        default:
            break;
    }
}

void ConsoleUI::reportEngineMove(const Move& /*move*/, int scoreCentipawns) {
    out_ << "Engine plays a move (evaluation: " << scoreCentipawns << " cp)\n";
}

} // namespace chess
