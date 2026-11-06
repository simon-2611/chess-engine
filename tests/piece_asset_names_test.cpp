#include <catch2/catch_test_macros.hpp>

#include "../src/qt/PieceAssetNames.h"
#include "../src/Types.h"

using namespace chess;

TEST_CASE("PieceAssetNames follows white_/black_ + type contract", "[qt][assets]") {
    REQUIRE(PieceAssetNames::stem({PieceType::Pawn, Color::White}) == "white_pawn");
    REQUIRE(PieceAssetNames::stem({PieceType::Knight, Color::White}) == "white_knight");
    REQUIRE(PieceAssetNames::stem({PieceType::Bishop, Color::White}) == "white_bishop");
    REQUIRE(PieceAssetNames::stem({PieceType::Rook, Color::White}) == "white_rook");
    REQUIRE(PieceAssetNames::stem({PieceType::Queen, Color::White}) == "white_queen");
    REQUIRE(PieceAssetNames::stem({PieceType::King, Color::White}) == "white_king");

    REQUIRE(PieceAssetNames::stem({PieceType::Pawn, Color::Black}) == "black_pawn");
    REQUIRE(PieceAssetNames::stem({PieceType::Knight, Color::Black}) == "black_knight");
    REQUIRE(PieceAssetNames::stem({PieceType::Bishop, Color::Black}) == "black_bishop");
    REQUIRE(PieceAssetNames::stem({PieceType::Rook, Color::Black}) == "black_rook");
    REQUIRE(PieceAssetNames::stem({PieceType::Queen, Color::Black}) == "black_queen");
    REQUIRE(PieceAssetNames::stem({PieceType::King, Color::Black}) == "black_king");
}

TEST_CASE("PieceAssetNames resource path uses pieces prefix and svg", "[qt][assets]") {
    REQUIRE(PieceAssetNames::resourcePath({PieceType::Queen, Color::Black}) ==
            ":/pieces/black_queen.svg");
    REQUIRE(PieceAssetNames::fileName({PieceType::King, Color::White}) == "white_king.svg");
}

TEST_CASE("PieceAssetNames empty piece has no asset", "[qt][assets]") {
    REQUIRE(PieceAssetNames::stem(Piece{}).empty());
    REQUIRE(PieceAssetNames::resourcePath(Piece{}).empty());
}
