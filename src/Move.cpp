#include "Move.h"

namespace chess {

Move::Move(Square from, Square to, PieceType movedPiece, PieceType capturedPiece, MoveFlag flag)
    : from_(from), to_(to), movedPiece_(movedPiece), capturedPiece_(capturedPiece), flag_(flag) {}

Square Move::from() const { return from_; }
Square Move::to() const { return to_; }
PieceType Move::movedPiece() const { return movedPiece_; }
PieceType Move::capturedPiece() const { return capturedPiece_; }
MoveFlag Move::flag() const { return flag_; }

bool Move::isCapture() const {
    return capturedPiece_ != PieceType::None || flag_ == MoveFlag::EnPassantCapture;
}

bool Move::isPromotion() const {
    return flag_ == MoveFlag::PromotionToQueen || flag_ == MoveFlag::PromotionToRook ||
           flag_ == MoveFlag::PromotionToBishop || flag_ == MoveFlag::PromotionToKnight;
}

bool Move::isCastling() const {
    return flag_ == MoveFlag::CastleKingSide || flag_ == MoveFlag::CastleQueenSide;
}

bool Move::operator==(const Move& other) const {
    return from_ == other.from_ && to_ == other.to_ && flag_ == other.flag_ &&
           movedPiece_ == other.movedPiece_ && capturedPiece_ == other.capturedPiece_;
}

bool Move::operator!=(const Move& other) const {
    return !(*this == other);
}

uint32_t Move::encode() const {
    // Layout (LSB -> MSB): from:8 | to:8 | movedPiece:4 | capturedPiece:4 | flag:4
    uint32_t encoded = 0;
    encoded |= (static_cast<uint32_t>(from_) & 0xFF);
    encoded |= (static_cast<uint32_t>(to_) & 0xFF) << 8;
    encoded |= (static_cast<uint32_t>(movedPiece_) & 0xF) << 16;
    encoded |= (static_cast<uint32_t>(capturedPiece_) & 0xF) << 20;
    encoded |= (static_cast<uint32_t>(flag_) & 0xF) << 24;
    return encoded;
}

Move Move::decode(uint32_t encoded) {
    Move move;
    move.from_ = static_cast<Square>(encoded & 0xFF);
    move.to_ = static_cast<Square>((encoded >> 8) & 0xFF);
    move.movedPiece_ = static_cast<PieceType>((encoded >> 16) & 0xF);
    move.capturedPiece_ = static_cast<PieceType>((encoded >> 20) & 0xF);
    move.flag_ = static_cast<MoveFlag>((encoded >> 24) & 0xF);
    return move;
}

} // namespace chess
