#include "GameControlsWidget.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace chess {

GameControlsWidget::GameControlsWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);

    statusLabel_ = new QLabel(tr("Ready."), this);
    statusLabel_->setWordWrap(true);
    layout->addWidget(statusLabel_);

    auto* form = new QFormLayout();
    whitePlayer_ = new QComboBox(this);
    whitePlayer_->addItem(tr("Human"));
    whitePlayer_->addItem(tr("Engine"));
    blackPlayer_ = new QComboBox(this);
    blackPlayer_->addItem(tr("Human"));
    blackPlayer_->addItem(tr("Engine"));
    // Default Human vs Human: AlphaBetaSearch currently returns an unset bestMove.
    blackPlayer_->setCurrentIndex(0);
    depthSpin_ = new QSpinBox(this);
    depthSpin_->setRange(1, 12);
    depthSpin_->setValue(4);

    form->addRow(tr("White"), whitePlayer_);
    form->addRow(tr("Black"), blackPlayer_);
    form->addRow(tr("Engine depth"), depthSpin_);
    layout->addLayout(form);

    newGameButton_ = new QPushButton(tr("New game"), this);
    undoButton_ = new QPushButton(tr("Undo"), this);
    resignButton_ = new QPushButton(tr("Resign"), this);
    quitButton_ = new QPushButton(tr("Quit"), this);

    layout->addWidget(newGameButton_);
    layout->addWidget(undoButton_);
    layout->addWidget(resignButton_);
    layout->addWidget(quitButton_);
    layout->addStretch(1);

    connect(newGameButton_, &QPushButton::clicked, this, &GameControlsWidget::newGameClicked);
    connect(undoButton_, &QPushButton::clicked, this, &GameControlsWidget::undoClicked);
    connect(resignButton_, &QPushButton::clicked, this, &GameControlsWidget::resignClicked);
    connect(quitButton_, &QPushButton::clicked, this, &GameControlsWidget::quitClicked);
    connect(whitePlayer_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int) { emitPlayerTypes(); });
    connect(blackPlayer_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this](int) { emitPlayerTypes(); });
    connect(depthSpin_, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &GameControlsWidget::searchDepthChanged);
}

void GameControlsWidget::setStatusText(const QString& text) {
    statusLabel_->setText(text);
}

QtGameRunner::PlayerType GameControlsWidget::playerTypeFromIndex(int index) const {
    return index == 1 ? QtGameRunner::PlayerType::Engine : QtGameRunner::PlayerType::Human;
}

void GameControlsWidget::emitPlayerTypes() {
    emit playerTypesChanged(playerTypeFromIndex(whitePlayer_->currentIndex()),
                            playerTypeFromIndex(blackPlayer_->currentIndex()));
}

} // namespace chess
