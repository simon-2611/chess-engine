#pragma once

#include <memory>
#include <unordered_map>

#include <QWidget>

#include "Board.h"
#include "Move.h"
#include "MoveSelectionModel.h"
#include "Types.h"

#if defined(CHESS_HAS_QT_SVG)
class QSvgRenderer;
#endif

namespace chess {

// Custom board view: click-select / click-move with selection and legal-move
// highlights. Piece SVGs are loaded from the :/pieces/ resource prefix when
// present; otherwise Unicode chess glyphs are used (no hand-drawn art).
class ChessBoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChessBoardWidget(QWidget* parent = nullptr);
    ~ChessBoardWidget() override;

    void setBoard(const Board& board);
    void setInputEnabled(bool enabled);
    void clearSelection();

signals:
    void moveChosen(const Move& move);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    int boardPixelSize() const;
    QRect boardRect() const;
    void ensureRenderersLoaded();
    void drawPiece(QPainter& painter, const QRect& rect, const Piece& piece) const;
    static QChar unicodeGlyph(const Piece& piece);

    Board board_{};
    MoveSelectionModel selection_;
    bool inputEnabled_ = false;
    bool whiteAtBottom_ = true;
    bool renderersLoaded_ = false;
#if defined(CHESS_HAS_QT_SVG)
    std::unordered_map<std::string, std::unique_ptr<QSvgRenderer>> renderers_;
#endif
};

} // namespace chess
