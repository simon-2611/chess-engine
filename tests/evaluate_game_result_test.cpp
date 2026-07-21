#include <catch2/catch_test_macros.hpp>
#include "../src/Board.h"

using namespace chess;

TEST_CASE("evaluateGameResult: Ongoing", "[board][evaluate]") {
    Board board = Board::startingPosition();
    // In starting position, White has legal moves and is not in checkmate/stalemate
    REQUIRE(board.evaluateGameResult(true) == GameResult::Ongoing);
}

TEST_CASE("evaluateGameResult: Checkmate", "[board][evaluate]") {
    // Fool's Mate
    // 1. f3 e5 2. g4 Qh4#
    Board board = Board::fromFEN("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 0 3");
    REQUIRE(board.isInCheck(Color::White));
    REQUIRE(board.evaluateGameResult(false) == GameResult::BlackWinsByCheckmate);
}

TEST_CASE("evaluateGameResult: Stalemate", "[board][evaluate]") {
    // Fastest stalemate
    Board board = Board::fromFEN("7k/5Q2/8/8/8/8/8/7K b - - 0 1");
    REQUIRE(!board.isInCheck(Color::Black));
    REQUIRE(board.evaluateGameResult(false) == GameResult::DrawByStalemate);
}

TEST_CASE("evaluateGameResult: 50-move rule", "[board][evaluate]") {
    Board board = Board::fromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 100 50");
    REQUIRE(board.evaluateGameResult(true) == GameResult::DrawByFiftyMoveRule);
}

TEST_CASE("evaluateGameResult: Insufficient material", "[board][evaluate]") {
    SECTION("KK") {
        Board board = Board::fromFEN("k7/8/8/8/8/8/8/K7 w - - 0 1");
        REQUIRE(board.evaluateGameResult(true) == GameResult::DrawByInsufficientMaterial);
    }
    SECTION("KN vs K") {
        Board board = Board::fromFEN("k7/8/8/8/8/8/8/KN6 w - - 0 1");
        REQUIRE(board.evaluateGameResult(true) == GameResult::DrawByInsufficientMaterial);
    }
    SECTION("KB vs K") {
        Board board = Board::fromFEN("k7/8/8/8/8/8/8/KB6 w - - 0 1");
        REQUIRE(board.evaluateGameResult(true) == GameResult::DrawByInsufficientMaterial);
    }
    SECTION("KB vs KB (same color bishops)") {
        // White King at a1, White Bishop at c1 (Black sq)
        // Black King at a8, Black Bishop at c8 (White sq) -- Wait
        // Let's use a1 and a3 for bishops.
        // a1 (0,0) -> Black
        // a3 (2,0) -> Black
        Board board = Board::fromFEN("k1b5/8/8/8/8/8/8/K1B5 w - - 0 1");
        // a8: k, b8: 1, c8: b. k is at file 0, rank 7. b is at file 2, rank 7. (2+7)%2 = 9%2 = 1 (White)
        // a1: K, b1: 1, c1: B. K is at 0,0. B is at file 2, rank 0. (2+0)%2 = 0 (Black)
        // One white, one black. Not same color.
        
        // Let's try again.
        // White B at c1 (2,0) -> Black
        // Black b at d2 (3,1) -> Black
        board = Board::fromFEN("8/8/8/8/8/8/3b4/2B1K1k1 w - - 0 1");
        // B at c1 (index 2): (0+2)%2 = 0 (Black)
        // b at d2 (index 11): (1+3)%2 = 0 (Black)
        REQUIRE(board.evaluateGameResult(true) == GameResult::DrawByInsufficientMaterial);
    }
    SECTION("KB vs KB (different color bishops)") {
        // a1 (0,0) -> 0 (Black)
        // b1 (0,1) -> 1 (White)
        Board board = Board::fromFEN("k7/8/8/8/8/8/8/KBB5 w - - 0 1");
        REQUIRE(board.evaluateGameResult(true) == GameResult::Ongoing);
    }
}

TEST_CASE("evaluateGameResult: Repetition", "[board][evaluate]") {
    Board board = Board::startingPosition();
    uint64_t h1 = board.zobristHash();
    
    // Simulate repetition
    std::vector<uint64_t> history = {h1, 123, h1, 456};
    // Current hash is h1. History has h1 twice. Total 3 times.
    REQUIRE(board.evaluateGameResult(true, history) == GameResult::DrawByRepetition);
    
    history = {h1, 123, 456};
    REQUIRE(board.evaluateGameResult(true, history) == GameResult::Ongoing);
}
