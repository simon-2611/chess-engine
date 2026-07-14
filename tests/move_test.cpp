#include <catch2/catch_test_macros.hpp>

#include "../src/Move.h"

using namespace chess;

TEST_CASE("Move encode/decode is lossless", "[move]") {
    Move original(12, 28, PieceType::Pawn, PieceType::None, MoveFlag::DoublePawnPush);

    uint32_t encoded = original.encode();
    Move decoded = Move::decode(encoded);

    REQUIRE(decoded.from() == original.from());
    REQUIRE(decoded.to() == original.to());
    REQUIRE(decoded.movedPiece() == original.movedPiece());
    REQUIRE(decoded.flag() == original.flag());
    REQUIRE(decoded == original);
}

TEST_CASE("isCapture recognizes normal capture moves", "[move]") {
    Move capture(20, 27, PieceType::Bishop, PieceType::Knight);
    Move quiet(20, 28, PieceType::Bishop, PieceType::None);

    REQUIRE(capture.isCapture());
    REQUIRE_FALSE(quiet.isCapture());
}
