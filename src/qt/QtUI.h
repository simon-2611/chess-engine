#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>

#include <QObject>
#include <QString>

#include "Board.h"
#include "InputCancelled.h"
#include "Move.h"
#include "Types.h"
#include "UserInterface.h"

namespace chess {

// Thread-bridge UserInterface: worker-thread GameController calls the blocking
// UserInterface methods; GUI-thread widgets connect to the signals and call
// submitHumanMove / cancelHumanInput. No QWidget is touched here.
//
// Cross-thread signal payloads use Qt-friendly types (FEN strings, encoded moves)
// so queued connections do not depend on Q_DECLARE_METATYPE for core chess types.
class QtUI : public QObject, public UserInterface {
    Q_OBJECT

public:
    explicit QtUI(QObject* parent = nullptr);

    void displayBoard(const Board& board) override;
    Move requestHumanMove(const Board& board) override;
    void announceResult(GameResult result) override;
    void reportEngineMove(const Move& move, int scoreCentipawns) override;

    // Called from the GUI thread when the user completes a move.
    void submitHumanMove(const Move& move);

    // Called from the GUI thread to abort a blocking requestHumanMove wait.
    void cancelHumanInput(InputCancelled::Reason reason);

    bool isAwaitingHumanMove() const;

signals:
    void boardUpdated(const QString& fen);
    void humanInputRequested(const QString& fen);
    void resultAnnounced(int result);
    void engineMoveReported(unsigned int encodedMove, int scoreCentipawns);
    void statusMessage(const QString& message);

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool awaitingHuman_ = false;
    std::optional<Move> submittedMove_;
    std::optional<InputCancelled::Reason> cancelReason_;
};

} // namespace chess
