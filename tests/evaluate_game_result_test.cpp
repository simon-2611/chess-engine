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
        Board board = Board::fromFEN("2k5/8/5b2/2B5/8/8/8/3K4 w - - 0 1");
        REQUIRE(board.evaluateGameResult(true) == GameResult::DrawByInsufficientMaterial);
    }
    SECTION("KB vs KB (different color bishops)") {
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
