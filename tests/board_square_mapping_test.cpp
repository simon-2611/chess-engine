#include <catch2/catch_test_macros.hpp>

#include "../src/qt/BoardSquareMapping.h"

using namespace chess;

TEST_CASE("BoardSquareMapping file/rank helpers", "[qt][mapping]") {
    REQUIRE(BoardSquareMapping::fileOf(0) == 0);   // a1
    REQUIRE(BoardSquareMapping::rankOf(0) == 0);
    REQUIRE(BoardSquareMapping::fileOf(63) == 7);  // h8
    REQUIRE(BoardSquareMapping::rankOf(63) == 7);
    REQUIRE(BoardSquareMapping::makeSquare(4, 1) == 12); // e2
    REQUIRE(BoardSquareMapping::makeSquare(4, 3) == 28); // e4
}

TEST_CASE("BoardSquareMapping squareAt with white at bottom", "[qt][mapping]") {
    constexpr int boardSize = 80; // 10px per square

    // Top-left pixel is a8 when white is at bottom
    REQUIRE(BoardSquareMapping::squareAt(0, 0, boardSize, true) == 56); // a8
    // Bottom-left is a1
    REQUIRE(BoardSquareMapping::squareAt(0, 79, boardSize, true) == 0); // a1
    // e2 center-ish
    REQUIRE(BoardSquareMapping::squareAt(45, 65, boardSize, true) == 12); // e2
    // Outside board
    REQUIRE(BoardSquareMapping::squareAt(-1, 0, boardSize, true) == kNoSquare);
    REQUIRE(BoardSquareMapping::squareAt(80, 0, boardSize, true) == kNoSquare);
}

TEST_CASE("BoardSquareMapping squareRect covers full square", "[qt][mapping]") {
    constexpr int boardSize = 80;
    auto rect = BoardSquareMapping::squareRect(0, boardSize, true); // a1 bottom-left
    REQUIRE(rect.x == 0);
    REQUIRE(rect.y == 70);
    REQUIRE(rect.width == 10);
    REQUIRE(rect.height == 10);

    auto a8 = BoardSquareMapping::squareRect(56, boardSize, true);
    REQUIRE(a8.x == 0);
    REQUIRE(a8.y == 0);
}

TEST_CASE("BoardSquareMapping algebraic labels", "[qt][mapping]") {
    REQUIRE(BoardSquareMapping::toAlgebraic(12) == "e2");
    REQUIRE(BoardSquareMapping::toAlgebraic(28) == "e4");
    REQUIRE(BoardSquareMapping::toAlgebraic(0) == "a1");
    REQUIRE(BoardSquareMapping::toAlgebraic(63) == "h8");
}
