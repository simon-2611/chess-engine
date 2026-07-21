#include <catch2/catch_test_macros.hpp>
#include "../src/ConsoleUI.h"
#include "../src/Board.h"
#include <sstream>

using namespace chess;

TEST_CASE("ConsoleUI::displayBoard", "[ui]") {
    std::stringstream out;
    std::stringstream in;
    ConsoleUI ui(in, out);
    Board board = Board::startingPosition();
    
    ui.displayBoard(board);
    
    std::string output = out.str();

    REQUIRE(output.find("r n b q k b n r") != std::string::npos); // Rank 8
    REQUIRE(output.find("R N B Q K B N R") != std::string::npos); // Rank 1
}

TEST_CASE("ConsoleUI::requestHumanMove - valid move", "[ui]") {
    std::stringstream out;
    std::stringstream in("e2e4\n");
    ConsoleUI ui(in, out);
    Board board = Board::startingPosition();
    
    Move move = ui.requestHumanMove(board);
    
    REQUIRE(move.from() == 12); // e2
    REQUIRE(move.to() == 28);   // e4
}

TEST_CASE("ConsoleUI::requestHumanMove - invalid then valid move", "[ui]") {
    std::stringstream out;
    // first "invalid", then "e2e4"
    std::stringstream in("invalid\ne2e4\n");
    ConsoleUI ui(in, out);
    Board board = Board::startingPosition();
    
    Move move = ui.requestHumanMove(board);
    
    REQUIRE(move.from() == 12); // e2
    REQUIRE(move.to() == 28);   // e4
    // Should have printed error message
    REQUIRE(out.str().find("Invalid move") != std::string::npos);
}

TEST_CASE("ConsoleUI::requestHumanMove - promotion", "[ui]") {
    std::stringstream out;
    std::stringstream in("a7a8q\n");
    ConsoleUI ui(in, out);
    // White pawn at a7, ready to promote.
    Board board = Board::fromFEN("8/P7/8/8/8/8/8/k6K w - - 0 1");
    
    Move move = ui.requestHumanMove(board);
    
    REQUIRE(move.from() == 48); // a7
    REQUIRE(move.to() == 56);   // a8
    REQUIRE(move.flag() == MoveFlag::PromotionToQueen);
}
