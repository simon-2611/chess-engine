#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <QObject>
#include <QString>
#include <QStringList>

#include "GameController.h"
#include "InputCancelled.h"
#include "Player.h"
#include "QtUI.h"
#include "SearchEngine.h"
#include "Types.h"

namespace chess {

// Owns GameController and runs the blocking game loop on a dedicated std::thread.
// Control methods are thread-safe and may be called directly from the GUI thread;
// they must not rely on a QThread event loop (which a forever started-slot would starve).
// QWidget objects are never touched here — only QtUI signals / atomics / controller state.
class QtGameRunner : public QObject {
    Q_OBJECT

public:
    enum class PlayerType {
        Human,
        Engine
    };

    explicit QtGameRunner(std::shared_ptr<QtUI> ui, QObject* parent = nullptr);
    ~QtGameRunner() override;

    // Starts the worker thread. Returns immediately (does not block the caller).
    void start();

    // Thread-safe controls — callable from the GUI thread at any time.
    void requestQuit();
    void requestNewGame();
    void requestUndo();
    void requestResign();
    void setPlayerTypes(PlayerType white, PlayerType black);
    void setSearchDepth(int depth);

    // Join the worker. timeoutMs < 0 waits indefinitely.
    // Returns false if still running when the timeout elapses.
    bool waitForFinished(int timeoutMs = -1);

    bool isRunning() const;
    PlayerType whitePlayerType() const;
    PlayerType blackPlayerType() const;

signals:
    void statusChanged(const QString& status);
    void historyChanged(const QStringList& moves);
    void gameFinished(const QString& resultText);
    void finished();

private:
    void configurePlayers();
    void runLoop();
    void publishHistory();
    void applyPendingControlsBeforeMove();
    void handleCancellation(InputCancelled::Reason reason);
    void handleInvalidEngineMove(const std::string& message);
    SearchLimits currentLimits() const;
    static QString resultToText(GameResult result);
    static QString moveToText(const Move& move);

    std::shared_ptr<QtUI> ui_;
    GameController controller_;

    std::thread worker_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stop_{false};
    std::atomic<bool> newGameRequested_{false};
    std::atomic<bool> undoRequested_{false};
    std::atomic<bool> resignRequested_{false};
    std::atomic<bool> sessionStopped_{false}; // resign / invalid engine / terminal UI stop
    std::atomic<PlayerType> whiteType_{PlayerType::Human};
    std::atomic<PlayerType> blackType_{PlayerType::Human};
    std::atomic<int> maxDepth_{4};

    mutable std::mutex doneMutex_;
    std::condition_variable doneCv_;
    bool workerDone_ = true;

    QString sessionStopText_;
};

} // namespace chess
