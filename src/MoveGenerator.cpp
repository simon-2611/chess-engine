#include "MoveGenerator.h"

namespace chess {

std::vector<Move> MoveGenerator::generatePseudoLegalMoves(const Board& board) {
    std::vector<Move> moves;
    Color us = board.sideToMove();
    Color them = oppositeColor(us);

    static const std::vector<int> knightOffsets = {-17, -15, -10, -6, 6, 10, 15, 17};
    static const std::vector<int> kingOffsets = {-9, -8, -7, -1, 1, 7, 8, 9};
    static const std::vector<int> rookOffsets = {8, -8, 1, -1};
    static const std::vector<int> bishopOffsets = {9, 7, -9, -7};
    static const std::vector<int> queenOffsets = {8, -8, 1, -1, 9, 7, -9, -7};

    for (int s = 0; s < 64; ++s) {
        Piece p = board.pieceAt(s);
        if (p.isEmpty() || p.color != us) continue;

        if (p.type == PieceType::Pawn) {
            int direction = (us == Color::White) ? 1 : -1;
            int startRank = (us == Color::White) ? 1 : 6;
            
            Square push1 = s + direction * 8;
            if (push1 >= 0 && push1 < 64 && board.pieceAt(push1).isEmpty()) {
                int push1Rank = push1 / 8;
                if (push1Rank == 0 || push1Rank == 7) {
                    moves.emplace_back(s, push1, PieceType::Pawn, PieceType::None, MoveFlag::PromotionToQueen);
                    moves.emplace_back(s, push1, PieceType::Pawn, PieceType::None, MoveFlag::PromotionToRook);
                    moves.emplace_back(s, push1, PieceType::Pawn, PieceType::None, MoveFlag::PromotionToBishop);
                    moves.emplace_back(s, push1, PieceType::Pawn, PieceType::None, MoveFlag::PromotionToKnight);
                } else {
                    moves.emplace_back(s, push1, PieceType::Pawn);
                    
                    if (s / 8 == startRank) {
                        Square push2 = s + direction * 16;
                        if (board.pieceAt(push2).isEmpty()) {
                            moves.emplace_back(s, push2, PieceType::Pawn, PieceType::None, MoveFlag::DoublePawnPush);
                        }
                    }
                }
            }
            
            for (int dx : {-1, 1}) {
                int file = s % 8;
                if ((file == 0 && dx == -1) || (file == 7 && dx == 1)) continue;
                Square target = s + direction * 8 + dx;
                if (target >= 0 && target < 64) {
                    Piece targetPiece = board.pieceAt(target);
                    if (!targetPiece.isEmpty() && targetPiece.color == them) {
                        int targetRank = target / 8;
                        if (targetRank == 0 || targetRank == 7) {
                            moves.emplace_back(s, target, PieceType::Pawn, targetPiece.type, MoveFlag::PromotionToQueen);
                            moves.emplace_back(s, target, PieceType::Pawn, targetPiece.type, MoveFlag::PromotionToRook);
                            moves.emplace_back(s, target, PieceType::Pawn, targetPiece.type, MoveFlag::PromotionToBishop);
                            moves.emplace_back(s, target, PieceType::Pawn, targetPiece.type, MoveFlag::PromotionToKnight);
                        } else {
                            moves.emplace_back(s, target, PieceType::Pawn, targetPiece.type);
                        }
                    } else if (target == board.enPassantSquare()) {
                        moves.emplace_back(s, target, PieceType::Pawn, PieceType::Pawn, MoveFlag::EnPassantCapture);
                    }
                }
            }
        } else if (p.type == PieceType::Knight) {
            for (int offset : knightOffsets) {
                Square target = s + offset;
                if (target >= 0 && target < 64 && std::abs(target % 8 - s % 8) <= 2) {
                    Piece targetPiece = board.pieceAt(target);
                    if (targetPiece.isEmpty() || targetPiece.color == them) {
                        moves.emplace_back(s, target, PieceType::Knight, targetPiece.type);
                    }
                }
            }
        } else if (p.type == PieceType::King) {
            for (int offset : kingOffsets) {
                Square target = s + offset;
                if (target >= 0 && target < 64 && std::abs(target % 8 - s % 8) <= 1) {
                    Piece targetPiece = board.pieceAt(target);
                    if (targetPiece.isEmpty() || targetPiece.color == them) {
                        moves.emplace_back(s, target, PieceType::King, targetPiece.type);
                    }
                }
            }

            // Castling
            if (us == Color::White) {
                if (board.castlingRights() & WhiteKingSide) {
                    if (board.pieceAt(5).isEmpty() && board.pieceAt(6).isEmpty()) {
                        if (!board.isSquareAttacked(4, Color::Black) &&
                            !board.isSquareAttacked(5, Color::Black) &&
                            !board.isSquareAttacked(6, Color::Black)) {
                            moves.emplace_back(4, 6, PieceType::King, PieceType::None, MoveFlag::CastleKingSide);
                        }
                    }
                }
                if (board.castlingRights() & WhiteQueenSide) {
                    if (board.pieceAt(1).isEmpty() && board.pieceAt(2).isEmpty() && board.pieceAt(3).isEmpty()) {
                        if (!board.isSquareAttacked(4, Color::Black) &&
                            !board.isSquareAttacked(3, Color::Black) &&
                            !board.isSquareAttacked(2, Color::Black)) {
                            moves.emplace_back(4, 2, PieceType::King, PieceType::None, MoveFlag::CastleQueenSide);
                        }
                    }
                }
            } else {
                if (board.castlingRights() & BlackKingSide) {
                    if (board.pieceAt(61).isEmpty() && board.pieceAt(62).isEmpty()) {
                        if (!board.isSquareAttacked(60, Color::White) &&
                            !board.isSquareAttacked(61, Color::White) &&
                            !board.isSquareAttacked(62, Color::White)) {
                            moves.emplace_back(60, 62, PieceType::King, PieceType::None, MoveFlag::CastleKingSide);
                        }
                    }
                }
                if (board.castlingRights() & BlackQueenSide) {
                    if (board.pieceAt(57).isEmpty() && board.pieceAt(58).isEmpty() && board.pieceAt(59).isEmpty()) {
                        if (!board.isSquareAttacked(60, Color::White) &&
                            !board.isSquareAttacked(59, Color::White) &&
                            !board.isSquareAttacked(58, Color::White)) {
                            moves.emplace_back(60, 58, PieceType::King, PieceType::None, MoveFlag::CastleQueenSide);
                        }
                    }
                }
            }
        } else {
            const auto& offsets = (p.type == PieceType::Rook) ? rookOffsets :
                                  (p.type == PieceType::Bishop) ? bishopOffsets : queenOffsets;
            for (int offset : offsets) {
                for (int i = 1; i < 8; ++i) {
                    Square target = s + offset * i;
                    if (target < 0 || target >= 64) break;
                    
                    // Edge wrap check
                    int prevSquare = s + offset * (i - 1);
                    if (std::abs(target % 8 - prevSquare % 8) > 1) break;

                    Piece targetPiece = board.pieceAt(target);
                    if (targetPiece.isEmpty()) {
                        moves.emplace_back(s, target, p.type);
                    } else {
                        if (targetPiece.color == them) {
                            moves.emplace_back(s, target, p.type, targetPiece.type);
                        }
                        break;
                    }
                }
            }
        }
    }

    return moves;
}

std::vector<Move> MoveGenerator::generateLegalMoves(const Board& board) {
    std::vector<Move> pseudoMoves = generatePseudoLegalMoves(board);
    std::vector<Move> legalMoves;
    Color us = board.sideToMove();

    Board tempBoard = board;
    for (const auto& m : pseudoMoves) {
        UndoInfo undo = tempBoard.makeMove(m);
        if (!tempBoard.isInCheck(us)) {
            legalMoves.push_back(m);
        }
        tempBoard.undoMove(m, undo);
    }

    return legalMoves;
}

std::vector<Move> MoveGenerator::generateCaptureMoves(const Board& board) {
    std::vector<Move> allLegal = generateLegalMoves(board);
    std::vector<Move> captures;
    for (const auto& m : allLegal) {
        if (m.isCapture()) {
            captures.push_back(m);
        }
    }
    return captures;
}

bool MoveGenerator::isMoveLegal(const Board& board, const Move& move) {
    std::vector<Move> legalMoves = generateLegalMoves(board);
    for (const auto& m : legalMoves) {
        if (m == move) return true;
    }
    return false;
}

bool MoveGenerator::hasAnyLegalMove(const Board& board) {
    return !generateLegalMoves(board).empty();
}

} // namespace chess
