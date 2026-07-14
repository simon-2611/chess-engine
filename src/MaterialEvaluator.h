#pragma once

#include "Evaluator.h"

namespace chess {

// Simplest useful evaluation function: sum of piece values.
// Good starting point to test the search independently of a
// more complex evaluation.
class MaterialEvaluator : public Evaluator {
public:
    int evaluate(const Board& board) const override;
    std::string name() const override;

private:
    static int pieceValue(PieceType type);
};

} // namespace chess
