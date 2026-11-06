#include <catch2/catch_test_macros.hpp>

#include <algorithm>

#include "../src/qt/MoveSelectionModel.h"
#include "../src/Board.h"
#include "../src/Move.h"

using namespace chess;

TEST_CASE("MoveSelectionModel ignores empty-square first click", "[qt][selection]") {
    MoveSelectionModel model;
    model.setPosition(Board::startingPosition());

    auto completed = model.handleSquareClick(16); // a3 empty
    REQUIRE_FALSE(completed.has_value());
    REQUIRE(model.selectedSquare() == kNoSquare);
    REQUIRE(model.legalDestinations().empty());
}

TEST_CASE("MoveSelectionModel selects own piece and lists legal destinations", "[qt][selection]") {
    MoveSelectionModel model;
    model.setPosition(Board::startingPosition());

    auto completed = model.handleSquareClick(12); // e2 white pawn
    REQUIRE_FALSE(completed.has_value());
    REQUIRE(model.selectedSquare() == 12);

    const auto dests = model.legalDestinations();
    REQUIRE(dests.size() == 2);
    REQUIRE(std::find(dests.begin(), dests.end(), 20) != dests.end()); // e3
    REQUIRE(std::find(dests.begin(), dests.end(), 28) != dests.end()); // e4
}

TEST_CASE("MoveSelectionModel completes a legal move on second click", "[qt][selection]") {
    MoveSelectionModel model;
    model.setPosition(Board::startingPosition());

    REQUIRE_FALSE(model.handleSquareClick(12).has_value());
    auto completed = model.handleSquareClick(28); // e2e4
    REQUIRE(completed.has_value());
    REQUIRE(completed->from() == 12);
    REQUIRE(completed->to() == 28);
    REQUIRE(model.selectedSquare() == kNoSquare);
}

TEST_CASE("MoveSelectionModel rejects illegal destination and keeps selection", "[qt][selection]") {
    MoveSelectionModel model;
    model.setPosition(Board::startingPosition());

    REQUIRE_FALSE(model.handleSquareClick(12).has_value());
    auto completed = model.handleSquareClick(13); // e2 to f2 illegal
    REQUIRE_FALSE(completed.has_value());
    // Clicking another own piece re-selects it instead
    REQUIRE(model.selectedSquare() == 13); // f2 white pawn
}

TEST_CASE("MoveSelectionModel deselects on second click of same square", "[qt][selection]") {
    MoveSelectionModel model;
    model.setPosition(Board::startingPosition());

    REQUIRE_FALSE(model.handleSquareClick(12).has_value());
    REQUIRE(model.selectedSquare() == 12);
    REQUIRE_FALSE(model.handleSquareClick(12).has_value());
    REQUIRE(model.selectedSquare() == kNoSquare);
}

TEST_CASE("MoveSelectionModel prefers queen promotion by default", "[qt][selection]") {
    MoveSelectionModel model;
    // White pawn a7 ready to promote; black king a8-ish safe FEN
    model.setPosition(Board::fromFEN("8/P7/8/8/8/8/8/k6K w - - 0 1"));

    REQUIRE_FALSE(model.handleSquareClick(48).has_value()); // a7
    auto completed = model.handleSquareClick(56); // a8
    REQUIRE(completed.has_value());
    REQUIRE(completed->flag() == MoveFlag::PromotionToQueen);
}

TEST_CASE("MoveSelectionModel clearSelection resets state", "[qt][selection]") {
    MoveSelectionModel model;
    model.setPosition(Board::startingPosition());
    model.handleSquareClick(12);
    REQUIRE(model.selectedSquare() == 12);
    model.clearSelection();
    REQUIRE(model.selectedSquare() == kNoSquare);
    REQUIRE(model.legalDestinations().empty());
}
