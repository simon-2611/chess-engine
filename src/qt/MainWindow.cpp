#include "MainWindow.h"

#include <QHBoxLayout>
#include <QCloseEvent>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

#include "BoardSquareMapping.h"
#include "ChessBoardWidget.h"
#include "EngineAnalysisPanel.h"
#include "GameControlsWidget.h"
#include "MoveHistoryPanel.h"

namespace chess {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    ui_ = std::make_shared<QtUI>();
    setupUi();
    startGameRunner();
}

MainWindow::~MainWindow() {
    shutdownGameRunner();
}

void MainWindow::setupUi() {
    setWindowTitle(tr("Chess Engine"));
    resize(960, 720);

    auto* central = new QWidget(this);
    auto* root = new QHBoxLayout(central);

    boardWidget_ = new ChessBoardWidget(central);
    root->addWidget(boardWidget_, 3);

    auto* side = new QVBoxLayout();
    controls_ = new GameControlsWidget(central);
    historyPanel_ = new MoveHistoryPanel(central);
    analysisPanel_ = new EngineAnalysisPanel(central);
    side->addWidget(controls_);
    side->addWidget(historyPanel_, 1);
    side->addWidget(analysisPanel_, 1);
    root->addLayout(side, 1);

    setCentralWidget(central);
    statusBar()->showMessage(tr("Starting…"));

    // QtUI may emit from the worker thread; force queued delivery onto the GUI thread.
    connect(ui_.get(), &QtUI::boardUpdated, this, &MainWindow::onBoardUpdated, Qt::QueuedConnection);
    connect(ui_.get(), &QtUI::humanInputRequested, this, &MainWindow::onHumanInputRequested,
            Qt::QueuedConnection);
    connect(ui_.get(), &QtUI::resultAnnounced, this, &MainWindow::onResultAnnounced,
            Qt::QueuedConnection);
    connect(ui_.get(), &QtUI::engineMoveReported, this, &MainWindow::onEngineMoveReported,
            Qt::QueuedConnection);

    connect(boardWidget_, &ChessBoardWidget::moveChosen, this, &MainWindow::onMoveChosen);

    // Runner controls are thread-safe atomics + cancelHumanInput. Call them directly from
    // the GUI thread — never via QueuedConnection onto a worker that might be blocked in
    // requestHumanMove / engine search (and must not depend on a starved QThread loop).
    connect(controls_, &GameControlsWidget::newGameClicked, this, [this] {
        boardWidget_->setInputEnabled(false);
        analysisPanel_->clear();
        if (runner_ != nullptr) {
            runner_->requestNewGame();
        }
    });
    connect(controls_, &GameControlsWidget::undoClicked, this, [this] {
        if (runner_ != nullptr) {
            runner_->requestUndo();
        }
    });
    connect(controls_, &GameControlsWidget::resignClicked, this, [this] {
        boardWidget_->setInputEnabled(false);
        if (runner_ != nullptr) {
            runner_->requestResign();
        }
    });
    connect(controls_, &GameControlsWidget::quitClicked, this, &QWidget::close);
    connect(controls_, &GameControlsWidget::playerTypesChanged, this,
            [this](QtGameRunner::PlayerType white, QtGameRunner::PlayerType black) {
                if (runner_ != nullptr) {
                    runner_->setPlayerTypes(white, black);
                }
            });
    connect(controls_, &GameControlsWidget::searchDepthChanged, this, [this](int depth) {
        if (runner_ != nullptr) {
            runner_->setSearchDepth(depth);
        }
    });

    // Future seam: SearchEngine has no cancellation API yet.
    connect(analysisPanel_, &EngineAnalysisPanel::cancelAnalysisRequested, this, [this] {
        analysisPanel_->setAnalysisText(
            tr("Analysis cancel requested (SearchEngine has no cancellation token yet)."));
    });
}

void MainWindow::startGameRunner() {
    // Runner QObject lives on the GUI thread (for signal affinity). The game loop runs on
    // an internal std::thread so controls never need a worker QEventLoop.
    runner_ = new QtGameRunner(ui_, this);

    connect(runner_, &QtGameRunner::statusChanged, this, &MainWindow::onStatusChanged,
            Qt::QueuedConnection);
    connect(runner_, &QtGameRunner::historyChanged, this, &MainWindow::onHistoryChanged,
            Qt::QueuedConnection);
    connect(runner_, &QtGameRunner::gameFinished, this, &MainWindow::onGameFinished,
            Qt::QueuedConnection);

    // Default remains Human vs Human (AlphaBetaSearch currently returns unset bestMove).
    runner_->start();
}

void MainWindow::shutdownGameRunner() {
    if (runner_ == nullptr) {
        return;
    }
    runner_->requestQuit();
    // May block until an in-flight engine search returns — core search is not cancellable.
    runner_->waitForFinished(-1);
    runner_ = nullptr; // owned by `this` via QObject parent; destroyed with MainWindow
}

void MainWindow::closeEvent(QCloseEvent* event) {
    shutdownGameRunner();
    event->accept();
}

void MainWindow::onBoardUpdated(const QString& fen) {
    boardWidget_->setBoard(Board::fromFEN(fen.toStdString()));
}

void MainWindow::onHumanInputRequested(const QString& fen) {
    boardWidget_->setBoard(Board::fromFEN(fen.toStdString()));
    boardWidget_->setInputEnabled(true);
    statusBar()->showMessage(tr("Your move…"));
}

void MainWindow::onResultAnnounced(int result) {
    boardWidget_->setInputEnabled(false);
    const QString text = resultToText(static_cast<GameResult>(result));
    controls_->setStatusText(text);
    statusBar()->showMessage(text);
}

void MainWindow::onEngineMoveReported(unsigned int encodedMove, int scoreCentipawns) {
    const Move move = Move::decode(encodedMove);
    const QString moveText =
        QString::fromStdString(BoardSquareMapping::toAlgebraic(move.from()) +
                               BoardSquareMapping::toAlgebraic(move.to()));
    analysisPanel_->reportEngineMove(moveText, scoreCentipawns);
}

void MainWindow::onMoveChosen(const Move& move) {
    boardWidget_->setInputEnabled(false);
    ui_->submitHumanMove(move);
}

void MainWindow::onStatusChanged(const QString& status) {
    controls_->setStatusText(status);
    statusBar()->showMessage(status);
}

void MainWindow::onHistoryChanged(const QStringList& moves) {
    historyPanel_->setMoves(moves);
}

void MainWindow::onGameFinished(const QString& resultText) {
    boardWidget_->setInputEnabled(false);
    controls_->setStatusText(resultText);
    statusBar()->showMessage(resultText);
}

QString MainWindow::resultToText(GameResult result) {
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

} // namespace chess
