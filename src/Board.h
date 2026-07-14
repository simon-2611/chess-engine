#pragma once

#include <array>
#include <string>
#include <vector>

#include "Move.h"
#include "Types.h"

namespace chess {

// Internal snapshot of irreversible state components (castling rights,
// en passant square, halfmove clock, captured piece). Needed for
// makeMove()/undoMove() without expensive board copies.
struct UndoInfo {
    uint8_t castlingRights = 0;
    Square enPassantSquare = kNoSquare;
    int halfmoveClock = 0;
    PieceType capturedPiece = PieceType::None;
};

// Represents exactly one chess position including all additional information
// relevant to the rules of moves. The internal storage (simple array
// vs. bitboards) is deliberately kept private so that it can be exchanged
// later without changing the public interface.
class Board {
public:
    Board();

    static Board startingPosition();
    static Board fromFEN(const std::string& fen);
    std::string toFEN() const;

    Piece pieceAt(Square square) const;
    Color sideToMove() const;
    uint8_t castlingRights() const;
    Square enPassantSquare() const;
    int halfmoveClock() const;
    int fullmoveNumber() const;

    // Executes a move (already assumed to be legal) or undoes it.
    // undoMove() requires the UndoInfo from the corresponding
    // makeMove() call, which is why the caller (e.g. the search engine)
    // must cache it.
    UndoInfo makeMove(const Move& move);
    void undoMove(const Move& move, const UndoInfo& undo);

    bool isSquareAttacked(Square square, Color byColor) const;
    bool isInCheck(Color color) const;

    GameResult evaluateGameResult(bool sideToMoveHasLegalMoves) const;

    // Zobrist hash of the current position, e.g. for the transposition table.
    uint64_t zobristHash() const;

private:
    std::array<Piece, 64> squares_{};
    Color sideToMove_ = Color::White;
    uint8_t castlingRights_ = 0;
    Square enPassantSquare_ = kNoSquare;
    int halfmoveClock_ = 0;
    int fullmoveNumber_ = 1;
};

} // namespace chess
