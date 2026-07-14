#pragma once

#include "Evaluator.h"

namespace chess {

// Extended evaluation: material plus piece-square tables, king safety
// and mobility. Shows by way of example how a second Evaluator
// implementation can be seamlessly exchanged for MaterialEvaluator without
// having to adjust SearchEngine or GameController.
class PositionalEvaluator : public Evaluator {
public:
    int evaluate(const Board& board) const override;
    std::string name() const override;

private:
    int materialScore(const Board& board) const;
    int pieceSquareScore(const Board& board) const;
    int kingSafetyScore(const Board& board) const;
    int mobilityScore(const Board& board) const;
};

} // namespace chess
