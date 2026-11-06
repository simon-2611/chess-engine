#pragma once

#include <QWidget>

class QLabel;
class QPlainTextEdit;
class QPushButton;

namespace chess {

// Placeholder panel for future engine analysis output.
// Exposes a cancellable-pattern seam (Cancel button) without tying the core
// search API to Qt yet.
class EngineAnalysisPanel : public QWidget {
    Q_OBJECT

public:
    explicit EngineAnalysisPanel(QWidget* parent = nullptr);

public slots:
    void setAnalysisText(const QString& text);
    void reportEngineMove(const QString& moveText, int scoreCentipawns);
    void clear();

signals:
    // Future seam: request cancellation of a long-running analysis/search.
    void cancelAnalysisRequested();

private:
    QLabel* title_ = nullptr;
    QPlainTextEdit* output_ = nullptr;
    QPushButton* cancelButton_ = nullptr;
};

} // namespace chess
