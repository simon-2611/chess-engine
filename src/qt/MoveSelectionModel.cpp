#include "MoveSelectionModel.h"

#include <algorithm>

#include "MoveGenerator.h"

namespace chess {

void MoveSelectionModel::setPosition(const Board& board) {
    board_ = board;
    legalMoves_ = MoveGenerator::generateLegalMoves(board_);
    clearSelection();
}

void MoveSelectionModel::clearSelection() {
    selected_ = kNoSquare;
    legalDestinations_.clear();
}

Square MoveSelectionModel::selectedSquare() const {
    return selected_;
}

const std::vector<Square>& MoveSelectionModel::legalDestinations() const {
    return legalDestinations_;
}

bool MoveSelectionModel::isLegalDestination(Square square) const {
    return std::find(legalDestinations_.begin(), legalDestinations_.end(), square) !=
           legalDestinations_.end();
}

void MoveSelectionModel::selectSquare(Square square) {
    selected_ = square;
    legalDestinations_.clear();
    for (const Move& move : legalMoves_) {
        if (move.from() == square) {
            if (std::find(legalDestinations_.begin(), legalDestinations_.end(), move.to()) ==
                legalDestinations_.end()) {
                legalDestinations_.push_back(move.to());
            }
        }
    }
}

std::optional<Move> MoveSelectionModel::findMove(Square from, Square to) const {
    const Move* promotionFallback = nullptr;
    for (const Move& move : legalMoves_) {
        if (move.from() != from || move.to() != to) {
            continue;
        }
        if (!move.isPromotion()) {
            return move;
        }
        if (move.flag() == MoveFlag::PromotionToQueen) {
            return move;
        }
        if (promotionFallback == nullptr) {
            promotionFallback = &move;
        }
    }
    if (promotionFallback != nullptr) {
        return *promotionFallback;
    }
    return std::nullopt;
}

std::optional<Move> MoveSelectionModel::handleSquareClick(Square square) {
    if (square < 0 || square > 63) {
        return std::nullopt;
    }

    if (selected_ == kNoSquare) {
        const Piece piece = board_.pieceAt(square);
        if (piece.isEmpty() || piece.color != board_.sideToMove()) {
            return std::nullopt;
        }
        // Only select pieces that have at least one legal move.
        bool hasMove = false;
        for (const Move& move : legalMoves_) {
            if (move.from() == square) {
                hasMove = true;
                break;
            }
        }
        if (!hasMove) {
            return std::nullopt;
        }
        selectSquare(square);
        return std::nullopt;
    }

    if (square == selected_) {
        clearSelection();
        return std::nullopt;
    }

    if (auto move = findMove(selected_, square)) {
        clearSelection();
        return move;
    }

    // Re-select another own piece if the click was not a legal destination.
    const Piece piece = board_.pieceAt(square);
    if (!piece.isEmpty() && piece.color == board_.sideToMove()) {
        bool hasMove = false;
        for (const Move& move : legalMoves_) {
            if (move.from() == square) {
                hasMove = true;
                break;
            }
        }
        if (hasMove) {
            selectSquare(square);
            return std::nullopt;
        }
    }

    // Illegal destination: keep the current selection.
    return std::nullopt;
}

} // namespace chess
