#pragma once

#include <QWidget>

class QListWidget;
class QLabel;

namespace chess {

// Simple move list panel (algebraic from-to pairs from the game runner).
class MoveHistoryPanel : public QWidget {
    Q_OBJECT

public:
    explicit MoveHistoryPanel(QWidget* parent = nullptr);

public slots:
    void setMoves(const QStringList& moves);
    void clear();

private:
    QLabel* title_ = nullptr;
    QListWidget* list_ = nullptr;
};

} // namespace chess
