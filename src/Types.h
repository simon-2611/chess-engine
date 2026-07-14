#pragma once

#include <cstdint>

namespace chess {

// A square is encoded as an index 0..63 (0 = a1, 63 = h8).
using Square = int;
constexpr Square kNoSquare = -1;

enum class Color : uint8_t {
    White = 0,
    Black = 1
};

inline Color oppositeColor(Color c) {
    return c == Color::White ? Color::Black : Color::White;
}

enum class PieceType : uint8_t {
    None = 0,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

// Combination of piece type and color, e.g. for the board representation.
struct Piece {
    PieceType type = PieceType::None;
    Color color = Color::White;

    bool isEmpty() const { return type == PieceType::None; }
};

// Return value of a finished or ongoing game.
enum class GameResult : uint8_t {
    Ongoing,
    WhiteWinsByCheckmate,
    BlackWinsByCheckmate,
    DrawByStalemate,
    DrawByFiftyMoveRule,
    DrawByRepetition,
    DrawByInsufficientMaterial
};

// Bitmask for remaining castling rights.
enum CastlingRights : uint8_t {
    None       = 0,
    WhiteKingSide  = 1 << 0,
    WhiteQueenSide = 1 << 1,
    BlackKingSide  = 1 << 2,
    BlackQueenSide = 1 << 3
};

} // namespace chess
