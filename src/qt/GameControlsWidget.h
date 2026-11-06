#pragma once

#include <QWidget>

#include "QtGameRunner.h"

class QComboBox;
class QLabel;
class QPushButton;
class QSpinBox;

namespace chess {

// New game / undo / resign / quit plus simple player-type selectors.
class GameControlsWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameControlsWidget(QWidget* parent = nullptr);

    void setStatusText(const QString& text);

signals:
    void newGameClicked();
    void undoClicked();
    void resignClicked();
    void quitClicked();
    void playerTypesChanged(QtGameRunner::PlayerType white, QtGameRunner::PlayerType black);
    void searchDepthChanged(int depth);

private:
    QtGameRunner::PlayerType playerTypeFromIndex(int index) const;
    void emitPlayerTypes();

    QLabel* statusLabel_ = nullptr;
    QComboBox* whitePlayer_ = nullptr;
    QComboBox* blackPlayer_ = nullptr;
    QSpinBox* depthSpin_ = nullptr;
    QPushButton* newGameButton_ = nullptr;
    QPushButton* undoButton_ = nullptr;
    QPushButton* resignButton_ = nullptr;
    QPushButton* quitButton_ = nullptr;
};

} // namespace chess
