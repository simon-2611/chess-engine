#include "TranspositionTable.h"

#include <algorithm>

namespace chess {

namespace {

std::size_t nextPowerOfTwo(std::size_t n) {
    std::size_t power = 1;
    while (power < n) {
        power *= 2;
    }
    return power;
}

} // namespace

TranspositionTable::TranspositionTable(std::size_t sizeInMegabytes) {
    std::size_t bytes = sizeInMegabytes * 1024ull * 1024ull;
    std::size_t entryCount = std::max<std::size_t>(1, bytes / sizeof(TranspositionEntry));
    entryCount = nextPowerOfTwo(entryCount);

    table_.resize(entryCount);
    indexMask_ = entryCount - 1;
}

void TranspositionTable::store(uint64_t zobristKey, int depth, int score,
                                NodeType type, const Move& bestMove) {
    std::size_t index = static_cast<std::size_t>(zobristKey) & indexMask_;
    TranspositionEntry& entry = table_[index];

    // Simple replacement strategy: overwrite free or shallower entry.
    if (!entry.isValid || depth >= entry.depth) {
        entry.key = zobristKey;
        entry.depth = depth;
        entry.score = score;
        entry.type = type;
        entry.bestMove = bestMove;
        entry.isValid = true;
    }
}

bool TranspositionTable::probe(uint64_t zobristKey, TranspositionEntry& outEntry) const {
    std::size_t index = static_cast<std::size_t>(zobristKey) & indexMask_;
    const TranspositionEntry& entry = table_[index];
    if (entry.isValid && entry.key == zobristKey) {
        outEntry = entry;
        return true;
    }
    return false;
}

void TranspositionTable::clear() {
    for (auto& entry : table_) {
        entry.isValid = false;
    }
}

} // namespace chess
