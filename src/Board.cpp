#include "Board.h"

namespace chess {

Board::Board() = default;

Board Board::startingPosition() {
    // TODO: Fill starting position (pawns, pieces on back ranks).
    Board board;
    board.sideToMove_ = Color::White;
    board.castlingRights_ = static_cast<uint8_t>(
        CastlingRights::WhiteKingSide | CastlingRights::WhiteQueenSide |
        CastlingRights::BlackKingSide | CastlingRights::BlackQueenSide);
    return board;
}

Board Board::fromFEN(const std::string& /*fen*/) {
    // TODO: Parse FEN string (piece placement, side to move, castling rights,
    // en passant square, halfmove/fullmove counter).
    return Board{};
}

std::string Board::toFEN() const {
    // TODO: Serialize current position as FEN string.
    return "";
}

Piece Board::pieceAt(Square /*square*/) const {
    return Piece{};
}

Color Board::sideToMove() const { return sideToMove_; }
uint8_t Board::castlingRights() const { return castlingRights_; }
Square Board::enPassantSquare() const { return enPassantSquare_; }
int Board::halfmoveClock() const { return halfmoveClock_; }
int Board::fullmoveNumber() const { return fullmoveNumber_; }

UndoInfo Board::makeMove(const Move& /*move*/) {
    // TODO: Move piece on squares_, handle special cases
    // (castling: move rook, en passant: remove captured pawn,
    // promotion: replace piece type), switch sideToMove_,
    // update castling rights/en passant square/halfmove clock.
    return UndoInfo{};
}

void Board::undoMove(const Move& /*move*/, const UndoInfo& /*undo*/) {
    // TODO: Fully undo makeMove(), including UndoInfo.
}

bool Board::isSquareAttacked(Square /*square*/, Color /*byColor*/) const {
    // TODO: Check if any opponent piece attacks 'square'.
    return false;
}

bool Board::isInCheck(Color /*color*/) const {
    // TODO: Determine king position, apply isSquareAttacked() on it.
    return false;
}

GameResult Board::evaluateGameResult(bool /*sideToMoveHasLegalMoves*/) const {
    // TODO: Check for checkmate/stalemate/50-move rule/repetition/insufficient material.
    return GameResult::Ongoing;
}

uint64_t Board::zobristHash() const {
    // TODO: Compose a hash from pre-generated random numbers per (square, piece, color)
    // as well as side to move/castling rights/en passant square.
    return 0;
}

} // namespace chess
