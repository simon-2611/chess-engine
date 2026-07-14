#include "MaterialEvaluator.h"

namespace chess {

int MaterialEvaluator::pieceValue(PieceType type) {
    switch (type) {
        case PieceType::Pawn:   return 100;
        case PieceType::Knight: return 320;
        case PieceType::Bishop: return 330;
        case PieceType::Rook:   return 500;
        case PieceType::Queen:  return 900;
        case PieceType::King:
        case PieceType::None:
        default:
            return 0;
    }
}

int MaterialEvaluator::evaluate(const Board& board) const {
    int score = 0;
    for (Square square = 0; square < 64; ++square) {
        Piece piece = board.pieceAt(square);
        if (piece.isEmpty()) {
            continue;
        }
        int value = pieceValue(piece.type);
        score += (piece.color == Color::White) ? value : -value;
    }
    return score;
}

std::string MaterialEvaluator::name() const {
    return "MaterialEvaluator";
}

} // namespace chess
