#pragma once

#include <cstdint>
#include <vector>

#include "Move.h"

namespace chess {

// Indicates whether the stored score is exact or represents only an
// upper/lower bound due to alpha-beta cutoffs.
enum class NodeType : uint8_t {
    Exact,
    LowerBound,
    UpperBound
};

struct TranspositionEntry {
    uint64_t key = 0;
    int depth = 0;
    int score = 0;
    NodeType type = NodeType::Exact;
    Move bestMove;
    bool isValid = false;
};

// Simple fixed-size hash table (power-of-two number of entries) to
// avoid redundant subtree calculations in alpha-beta searches.
class TranspositionTable {
public:
    explicit TranspositionTable(std::size_t sizeInMegabytes);

    void store(uint64_t zobristKey, int depth, int score,
               NodeType type, const Move& bestMove);

    // Returns true and fills 'outEntry' if a matching entry
    // was found.
    bool probe(uint64_t zobristKey, TranspositionEntry& outEntry) const;

    void clear();

private:
    std::vector<TranspositionEntry> table_;
    std::size_t indexMask_ = 0;
};

} // namespace chess
