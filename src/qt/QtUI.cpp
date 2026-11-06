#include "QtUI.h"

namespace chess {

QtUI::QtUI(QObject* parent) : QObject(parent) {}

void QtUI::displayBoard(const Board& board) {
    emit boardUpdated(QString::fromStdString(board.toFEN()));
}

Move QtUI::requestHumanMove(const Board& board) {
    {
        std::lock_guard lock(mutex_);
        awaitingHuman_ = true;
        submittedMove_.reset();
        cancelReason_.reset();
    }

    emit humanInputRequested(QString::fromStdString(board.toFEN()));

    std::unique_lock lock(mutex_);
    cv_.wait(lock, [this] {
        return submittedMove_.has_value() || cancelReason_.has_value();
    });

    awaitingHuman_ = false;

    if (cancelReason_.has_value()) {
        const auto reason = *cancelReason_;
        cancelReason_.reset();
        submittedMove_.reset();
        throw InputCancelled(reason);
    }

    Move move = *submittedMove_;
    submittedMove_.reset();
    return move;
}

void QtUI::announceResult(GameResult result) {
    emit resultAnnounced(static_cast<int>(result));
}

void QtUI::reportEngineMove(const Move& move, int scoreCentipawns) {
    emit engineMoveReported(move.encode(), scoreCentipawns);
}

void QtUI::submitHumanMove(const Move& move) {
    {
        std::lock_guard lock(mutex_);
        if (!awaitingHuman_) {
            return;
        }
        submittedMove_ = move;
        cancelReason_.reset();
    }
    cv_.notify_all();
}

void QtUI::cancelHumanInput(InputCancelled::Reason reason) {
    {
        std::lock_guard lock(mutex_);
        if (!awaitingHuman_) {
            return;
        }
        cancelReason_ = reason;
        submittedMove_.reset();
    }
    cv_.notify_all();
}

bool QtUI::isAwaitingHumanMove() const {
    std::lock_guard lock(mutex_);
    return awaitingHuman_;
}

} // namespace chess
