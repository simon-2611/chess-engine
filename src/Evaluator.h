#pragma once

#include <string>

#include "Board.h"

namespace chess {

// Abstract base class for evaluation functions. Positive values are good
// from the perspective of Color::White, negative from the perspective of
// Color::Black (usual convention, saves sign switching in Negamax searches).
class Evaluator {
public:
    virtual ~Evaluator() = default;

    virtual int evaluate(const Board& board) const = 0;

    // Only for logging/debugging, e.g. to show in the UI which
    // evaluation strategy is currently active.
    virtual std::string name() const = 0;
};

} // namespace chess
