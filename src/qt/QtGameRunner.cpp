#include "QtGameRunner.h"

#include <chrono>
#include <utility>

#include "AlphaBetaSearch.h"
#include "BoardSquareMapping.h"
#include "MaterialEvaluator.h"
#include "MoveGenerator.h"
#include "ValidatingEnginePlayer.h"

namespace chess {

QtGameRunner::QtGameRunner(std::shared_ptr<QtUI> ui, QObject* parent)
    : QObject(parent), ui_(std::move(ui)), controller_(ui_) {
    configurePlayers();
}

QtGameRunner::~QtGameRunner() {
    requestQuit();
    // Best-effort join. Engine searches are not cancellable in core SearchEngine.
    waitForFinished(-1);
}

void QtGameRunner::setPlayerTypes(PlayerType white, PlayerType black) {
    whiteType_ = white;
    blackType_ = black;
    newGameRequested_ = true;
    sessionStopped_ = false;
    if (ui_) {
        ui_->cancelHumanInput(InputCancelled::Reason::NewGame);
    }
}

void QtGameRunner::setSearchDepth(int depth) {
    if (depth < 1) {
        depth = 1;
    }
    if (depth > 12) {
        depth = 12;
    }
    maxDepth_ = depth;
}

QtGameRunner::PlayerType QtGameRunner::whitePlayerType() const {
    return whiteType_.load();
}

QtGameRunner::PlayerType QtGameRunner::blackPlayerType() const {
    return blackType_.load();
}

SearchLimits QtGameRunner::currentLimits() const {
    SearchLimits limits;
    limits.maxDepth = maxDepth_.load();
    return limits;
}

void QtGameRunner::configurePlayers() {
    auto makePlayer = [this](PlayerType type) -> std::unique_ptr<Player> {
        if (type == PlayerType::Engine) {
            auto evaluator = std::make_shared<MaterialEvaluator>();
            auto engine = std::make_unique<AlphaBetaSearch>(evaluator);
            // Never feed Board::makeMove an unset/illegal engine move.
            return std::make_unique<ValidatingEnginePlayer>(std::move(engine));
        }
        return std::make_unique<HumanPlayer>(ui_);
    };
    controller_.setWhitePlayer(makePlayer(whiteType_.load()));
    controller_.setBlackPlayer(makePlayer(blackType_.load()));
}

void QtGameRunner::start() {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) {
        return;
    }

    {
        std::lock_guard lock(doneMutex_);
        workerDone_ = false;
    }

    stop_ = false;
    sessionStopped_ = false;
    newGameRequested_ = false;
    undoRequested_ = false;
    resignRequested_ = false;

    if (worker_.joinable()) {
        worker_.join();
    }

    worker_ = std::thread([this] {
        try {
            runLoop();
        } catch (...) {
            // Keep process alive; surface a generic stop.
            emit statusChanged(tr("Game worker stopped unexpectedly."));
        }
        {
            std::lock_guard lock(doneMutex_);
            workerDone_ = true;
            running_ = false;
        }
        doneCv_.notify_all();
        emit finished();
    });
}

bool QtGameRunner::isRunning() const {
    return running_.load();
}

bool QtGameRunner::waitForFinished(int timeoutMs) {
    std::unique_lock lock(doneMutex_);
    if (workerDone_ && !worker_.joinable()) {
        return true;
    }

    if (timeoutMs < 0) {
        doneCv_.wait(lock, [this] { return workerDone_; });
    } else {
        const bool ok = doneCv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                         [this] { return workerDone_; });
        if (!ok) {
            return false;
        }
    }

    lock.unlock();
    if (worker_.joinable()) {
        worker_.join();
    }
    return true;
}

void QtGameRunner::requestQuit() {
    stop_ = true;
    newGameRequested_ = false;
    undoRequested_ = false;
    resignRequested_ = false;
    if (ui_) {
        ui_->cancelHumanInput(InputCancelled::Reason::Quit);
    }
}

void QtGameRunner::requestNewGame() {
    newGameRequested_ = true;
    undoRequested_ = false;
    resignRequested_ = false;
    sessionStopped_ = false;
    if (ui_) {
        ui_->cancelHumanInput(InputCancelled::Reason::NewGame);
    }
}

void QtGameRunner::requestUndo() {
    if (stop_ || sessionStopped_) {
        return;
    }
    undoRequested_ = true;
    if (ui_) {
        ui_->cancelHumanInput(InputCancelled::Reason::Undo);
    }
}

void QtGameRunner::requestResign() {
    if (stop_ || sessionStopped_) {
        return;
    }
    resignRequested_ = true;
    if (ui_) {
        ui_->cancelHumanInput(InputCancelled::Reason::Resign);
    }
}

void QtGameRunner::handleCancellation(InputCancelled::Reason reason) {
    switch (reason) {
        case InputCancelled::Reason::Undo:
            undoRequested_ = false;
            if (controller_.undoLastHalfMove()) {
                // Undo opponent half-move as well when playing human vs engine
                // so the human is on move again when possible.
                if ((whiteType_ == PlayerType::Human) != (blackType_ == PlayerType::Human)) {
                    controller_.undoLastHalfMove();
                }
                ui_->displayBoard(controller_.board());
                publishHistory();
                emit statusChanged(tr("Move undone."));
            }
            break;
        case InputCancelled::Reason::NewGame:
            newGameRequested_ = false;
            sessionStopped_ = false;
            sessionStopText_.clear();
            configurePlayers();
            controller_.newGame();
            ui_->displayBoard(controller_.board());
            publishHistory();
            emit statusChanged(tr("New game."));
            break;
        case InputCancelled::Reason::Resign: {
            // UI-level resign only: core GameResult has no Resign value and UserInterface
            // is left unchanged, so GameController still reports Ongoing.
            resignRequested_ = false;
            const Color side = controller_.board().sideToMove();
            sessionStopText_ = (side == Color::White)
                                   ? tr("White resigns. Black wins. (UI-level result; core still Ongoing)")
                                   : tr("Black resigns. White wins. (UI-level result; core still Ongoing)");
            sessionStopped_ = true;
            emit statusChanged(sessionStopText_);
            emit gameFinished(sessionStopText_);
            break;
        }
        case InputCancelled::Reason::Quit:
            stop_ = true;
            break;
    }
}

void QtGameRunner::handleInvalidEngineMove(const std::string& message) {
    sessionStopText_ =
        tr("Engine returned an illegal/unset move and was stopped for safety: %1")
            .arg(QString::fromStdString(message));
    sessionStopped_ = true;
    emit statusChanged(sessionStopText_);
    emit gameFinished(sessionStopText_);
}

void QtGameRunner::publishHistory() {
    QStringList moves;
    const auto& history = controller_.history();
    for (const HistoryEntry& entry : history) {
        moves.push_back(moveToText(entry.move));
    }
    emit historyChanged(moves);
}

QString QtGameRunner::moveToText(const Move& move) {
    QString text = QString::fromStdString(BoardSquareMapping::toAlgebraic(move.from()) +
                                          BoardSquareMapping::toAlgebraic(move.to()));
    if (move.isPromotion()) {
        switch (move.flag()) {
            case MoveFlag::PromotionToQueen:  text += 'q'; break;
            case MoveFlag::PromotionToRook:   text += 'r'; break;
            case MoveFlag::PromotionToBishop: text += 'b'; break;
            case MoveFlag::PromotionToKnight: text += 'n'; break;
            default: break;
        }
    }
    return text;
}

QString QtGameRunner::resultToText(GameResult result) {
    switch (result) {
        case GameResult::WhiteWinsByCheckmate: return QStringLiteral("White wins by checkmate.");
        case GameResult::BlackWinsByCheckmate: return QStringLiteral("Black wins by checkmate.");
        case GameResult::DrawByStalemate: return QStringLiteral("Draw by stalemate.");
        case GameResult::DrawByFiftyMoveRule: return QStringLiteral("Draw by 50-move rule.");
        case GameResult::DrawByRepetition: return QStringLiteral("Draw by repetition.");
        case GameResult::DrawByInsufficientMaterial:
            return QStringLiteral("Draw by insufficient material.");
        case GameResult::Ongoing:
        default:
            return QStringLiteral("Game ongoing.");
    }
}

void QtGameRunner::applyPendingControlsBeforeMove() {
    if (stop_) {
        return;
    }
    if (newGameRequested_.exchange(false)) {
        // NewGame has priority over Resign/Undo.
        undoRequested_ = false;
        resignRequested_ = false;
        handleCancellation(InputCancelled::Reason::NewGame);
    } else if (resignRequested_.exchange(false)) {
        // Resign has priority over Undo.
        undoRequested_ = false;
        handleCancellation(InputCancelled::Reason::Resign);
    } else if (undoRequested_.exchange(false)) {
        handleCancellation(InputCancelled::Reason::Undo);
    }
}

void QtGameRunner::runLoop() {
    configurePlayers();
    controller_.newGame();
    ui_->displayBoard(controller_.board());
    publishHistory();
    emit statusChanged(tr("White to move."));

    while (!stop_) {
        applyPendingControlsBeforeMove();
        if (stop_) {
            break;
        }

        if (sessionStopped_.load()) {
            // Wait for new game / quit. Sleep so we do not spin hot; controls use atomics.
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        if (controller_.currentResult() != GameResult::Ongoing) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        const Color side = controller_.board().sideToMove();
        emit statusChanged(side == Color::White ? tr("White to move.") : tr("Black to move."));

        try {
            // Note: core SearchEngine has no cancellation token. If an engine search is
            // in progress, requestQuit/NewGame/Resign cannot interrupt it until findBestMove
            // returns. reportEngineMove is also not invoked by GameController/EnginePlayer today.
            controller_.playNextHalfMove(currentLimits());
            publishHistory();

            if (controller_.currentResult() != GameResult::Ongoing) {
                const QString text = resultToText(controller_.currentResult());
                emit statusChanged(text);
                emit gameFinished(text);
            }
        } catch (const InputCancelled& ex) {
            handleCancellation(ex.reason());
        } catch (const InvalidEngineMove& ex) {
            handleInvalidEngineMove(ex.what());
        }
    }
}

} // namespace chess
