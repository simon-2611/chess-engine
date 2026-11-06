#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <memory>

#include "../src/Board.h"
#include "../src/MaterialEvaluator.h"
#include "../src/MoveGenerator.h"
#include "../src/SearchEngine.h"
#include "../src/qt/ValidatingEnginePlayer.h"

using namespace chess;

namespace {

class StubUnsetMoveEngine : public SearchEngine {
public:
    StubUnsetMoveEngine()
        : SearchEngine(std::make_shared<MaterialEvaluator>()) {}

    SearchResult findBestMove(Board& board, const SearchLimits& limits) override {
        (void)board;
        (void)limits;
        SearchResult result;
        // Deliberately leave bestMove default-constructed (unset).
        result.score = 0;
        return result;
    }
};

class StubLegalFirstMoveEngine : public SearchEngine {
public:
    StubLegalFirstMoveEngine()
        : SearchEngine(std::make_shared<MaterialEvaluator>()) {}

    SearchResult findBestMove(Board& board, const SearchLimits& limits) override {
        (void)limits;
        SearchResult result;
        const auto legal = MoveGenerator::generateLegalMoves(board);
        REQUIRE_FALSE(legal.empty());
        result.bestMove = legal.front();
        result.score = 12;
        return result;
    }
};

} // namespace

TEST_CASE("ValidatingEnginePlayer rejects unset/illegal engine bestMove",
          "[qt][engine-guard]") {
    Board board = Board::startingPosition();
    SearchLimits limits;
    limits.maxDepth = 1;

    ValidatingEnginePlayer player(std::make_unique<StubUnsetMoveEngine>());
    REQUIRE_THROWS_AS(player.getMove(board, limits), InvalidEngineMove);
}

TEST_CASE("ValidatingEnginePlayer accepts a legal engine move", "[qt][engine-guard]") {
    Board board = Board::startingPosition();
    SearchLimits limits;
    limits.maxDepth = 1;

    ValidatingEnginePlayer player(std::make_unique<StubLegalFirstMoveEngine>());
    const Move move = player.getMove(board, limits);
    const auto legal = MoveGenerator::generateLegalMoves(board);
    REQUIRE(std::find(legal.begin(), legal.end(), move) != legal.end());
}
