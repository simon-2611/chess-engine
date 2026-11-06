#pragma once

#include <memory>

#include <QMainWindow>

#include "QtGameRunner.h"
#include "QtUI.h"

namespace chess {

class ChessBoardWidget;
class MoveHistoryPanel;
class EngineAnalysisPanel;
class GameControlsWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onBoardUpdated(const QString& fen);
    void onHumanInputRequested(const QString& fen);
    void onResultAnnounced(int result);
    void onEngineMoveReported(unsigned int encodedMove, int scoreCentipawns);
    void onMoveChosen(const Move& move);
    void onStatusChanged(const QString& status);
    void onHistoryChanged(const QStringList& moves);
    void onGameFinished(const QString& resultText);

private:
    void setupUi();
    void startGameRunner();
    void shutdownGameRunner();
    static QString resultToText(GameResult result);

    std::shared_ptr<QtUI> ui_;
    ChessBoardWidget* boardWidget_ = nullptr;
    MoveHistoryPanel* historyPanel_ = nullptr;
    EngineAnalysisPanel* analysisPanel_ = nullptr;
    GameControlsWidget* controls_ = nullptr;
    QtGameRunner* runner_ = nullptr;
};

} // namespace chess
