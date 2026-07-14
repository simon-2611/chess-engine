#pragma once

#include "Types.h"

namespace chess {

// Special cases that must be stored beyond origin/target square.
enum class MoveFlag : uint8_t {
    Normal,
    DoublePawnPush,
    EnPassantCapture,
    CastleKingSide,
    CastleQueenSide,
    PromotionToQueen,
    PromotionToRook,
    PromotionToBishop,
    PromotionToKnight
};

// A move is deliberately kept as a simple value type (no pointer to the
// board), so that move lists can be quickly copied and undone
// during the search.
class Move {
public:
    Move() = default;
    Move(Square from, Square to, PieceType movedPiece,
         PieceType capturedPiece = PieceType::None,
         MoveFlag flag = MoveFlag::Normal);

    Square from() const;
    Square to() const;
    PieceType movedPiece() const;
    PieceType capturedPiece() const;
    MoveFlag flag() const;

    bool isCapture() const;
    bool isPromotion() const;
    bool isCastling() const;

    bool operator==(const Move& other) const;
    bool operator!=(const Move& other) const;

    // Compact encoding for transposition table entries / move ordering.
    uint32_t encode() const;
    static Move decode(uint32_t encoded);

private:
    Square from_ = kNoSquare;
    Square to_ = kNoSquare;
    PieceType movedPiece_ = PieceType::None;
    PieceType capturedPiece_ = PieceType::None;
    MoveFlag flag_ = MoveFlag::Normal;
};

} // namespace chess
