#include "MoveHistoryPanel.h"

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

namespace chess {

MoveHistoryPanel::MoveHistoryPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    title_ = new QLabel(tr("Move history"), this);
    list_ = new QListWidget(this);
    list_->setAlternatingRowColors(true);
    layout->addWidget(title_);
    layout->addWidget(list_, 1);
}

void MoveHistoryPanel::setMoves(const QStringList& moves) {
    list_->clear();
    int ply = 0;
    for (const QString& move : moves) {
        ++ply;
        const int moveNumber = (ply + 1) / 2;
        if (ply % 2 == 1) {
            list_->addItem(QStringLiteral("%1. %2").arg(moveNumber).arg(move));
        } else {
            list_->addItem(QStringLiteral("%1... %2").arg(moveNumber).arg(move));
        }
    }
    if (list_->count() > 0) {
        list_->scrollToBottom();
    }
}

void MoveHistoryPanel::clear() {
    list_->clear();
}

} // namespace chess
