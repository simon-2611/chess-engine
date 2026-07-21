#include <catch2/catch_test_macros.hpp>
#include "../src/MoveGenerator.h"
#include "../src/Board.h"
#include <algorithm>

using namespace chess;

TEST_CASE("MoveGenerator::generatePseudoLegalMoves - White Pawn start", "[movegen]") {
    Board board = Board::fromFEN("8/8/8/8/8/8/4P3/8 w - - 0 1");
    auto moves = MoveGenerator::generatePseudoLegalMoves(board);
    
    // e2 (12) can go to e3 (20) and e4 (28)
    REQUIRE(moves.size() == 2);
}

TEST_CASE("MoveGenerator::generatePseudoLegalMoves - Sliding pieces", "[movegen]") {
    // White Rook at d4
    Board board = Board::fromFEN("8/8/8/8/3R4/8/8/8 w - - 0 1");
    auto moves = MoveGenerator::generatePseudoLegalMoves(board);
    
    // d4 is square 27.
    // Rook can move along d-file (7 squares other than d4) and 4th rank (7 squares other than d4).
    // Total 14 moves.
    REQUIRE(moves.size() == 14);
}

TEST_CASE("MoveGenerator::generateLegalMoves - King in check", "[movegen]") {
    // White king at e1 (4), Black rook at e8 (60). White must move king or block.
    Board board = Board::fromFEN("4r3/8/8/8/8/8/8/4K3 w - - 0 1");
    
    // We need to make sure isInCheck and isSquareAttacked work.
    // If they don't work, this test might pass but for the wrong reasons if it returns 0 moves.
    auto moves = MoveGenerator::generateLegalMoves(board);
    
    // King at e1 is in check by Rook at e8.
    // King can move to d1, f1, d2, f2.
    // e2 is NOT legal because it's still on the e-file.
    
    REQUIRE(!moves.empty());
    for (const auto& m : moves) {
        REQUIRE(m.movedPiece() == PieceType::King);
        REQUIRE(m.to() % 8 != 4); // Not on e-file
    }
}
