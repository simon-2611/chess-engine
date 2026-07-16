#include <catch2/catch_test_macros.hpp>
#include "../src/Board.h"

using namespace chess;

TEST_CASE("Board::toFEN() starting position", "[board][fen]") {
    Board board = Board::startingPosition();
    REQUIRE(board.toFEN() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

TEST_CASE("Board::fromFEN() and Board::toFEN() roundtrip", "[board][fen]") {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Board board = Board::fromFEN(fen);
    REQUIRE(board.toFEN() == fen);

    fen = "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 1 3";
    board = Board::fromFEN(fen);
    REQUIRE(board.toFEN() == fen);
    
    fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    board = Board::fromFEN(fen);
    REQUIRE(board.toFEN() == fen);
}

TEST_CASE("Board::fromFEN() parsing details", "[board][fen]") {
    std::string fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2";
    Board board = Board::fromFEN(fen);
    
    REQUIRE(board.sideToMove() == Color::White);
    REQUIRE(board.castlingRights() == (WhiteKingSide | WhiteQueenSide | BlackKingSide | BlackQueenSide));
    REQUIRE(board.enPassantSquare() != kNoSquare); // c6 square
    REQUIRE(board.enPassantSquare() == 42); // c6: 5*8 + 2 = 42
    REQUIRE(board.halfmoveClock() == 0);
    REQUIRE(board.fullmoveNumber() == 2);

    // Check pieces
    REQUIRE(board.pieceAt(0).type == PieceType::Rook);
    REQUIRE(board.pieceAt(0).color == Color::White);
    REQUIRE(board.pieceAt(63).type == PieceType::Rook);
    REQUIRE(board.pieceAt(63).color == Color::Black);
    // c5: file 2 (a=0, b=1, c=2), rank 4 (1=0, 2=1, 3=2, 4=3, 5=4). 4*8 + 2 = 34.
    REQUIRE(board.pieceAt(34).type == PieceType::Pawn);
    REQUIRE(board.pieceAt(34).color == Color::Black);
}
