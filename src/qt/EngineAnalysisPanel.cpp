#include "EngineAnalysisPanel.h"

#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace chess {

EngineAnalysisPanel::EngineAnalysisPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    title_ = new QLabel(tr("Engine analysis"), this);
    output_ = new QPlainTextEdit(this);
    output_->setReadOnly(true);
    output_->setPlaceholderText(tr("Engine output will appear here (future analysis seam)."));
    cancelButton_ = new QPushButton(tr("Cancel analysis"), this);
    cancelButton_->setEnabled(false);
    cancelButton_->setToolTip(tr("Reserved for cancellable engine analysis."));

    layout->addWidget(title_);
    layout->addWidget(output_, 1);
    layout->addWidget(cancelButton_);

    connect(cancelButton_, &QPushButton::clicked, this, &EngineAnalysisPanel::cancelAnalysisRequested);
}

void EngineAnalysisPanel::setAnalysisText(const QString& text) {
    output_->setPlainText(text);
    cancelButton_->setEnabled(!text.isEmpty());
}

void EngineAnalysisPanel::reportEngineMove(const QString& moveText, int scoreCentipawns) {
    output_->appendPlainText(
        tr("Engine move %1 (%2 cp)").arg(moveText).arg(scoreCentipawns));
    cancelButton_->setEnabled(true);
}

void EngineAnalysisPanel::clear() {
    output_->clear();
    cancelButton_->setEnabled(false);
}

} // namespace chess
