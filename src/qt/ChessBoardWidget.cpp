#include "ChessBoardWidget.h"

#include <algorithm>

#include <QFile>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#if defined(CHESS_HAS_QT_SVG)
#include <QSvgRenderer>
#endif

#include "BoardSquareMapping.h"
#include "PieceAssetNames.h"

namespace chess {

ChessBoardWidget::ChessBoardWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(320, 320);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    board_ = Board::startingPosition();
    selection_.setPosition(board_);
}

ChessBoardWidget::~ChessBoardWidget() = default;

void ChessBoardWidget::setBoard(const Board& board) {
    board_ = board;
    selection_.setPosition(board_);
    update();
}

void ChessBoardWidget::setInputEnabled(bool enabled) {
    inputEnabled_ = enabled;
    if (!enabled) {
        selection_.clearSelection();
        update();
    }
}

void ChessBoardWidget::clearSelection() {
    selection_.clearSelection();
    update();
}

int ChessBoardWidget::boardPixelSize() const {
    return std::min(width(), height());
}

QRect ChessBoardWidget::boardRect() const {
    const int size = boardPixelSize();
    const int x = (width() - size) / 2;
    const int y = (height() - size) / 2;
    return {x, y, size, size};
}

void ChessBoardWidget::ensureRenderersLoaded() {
    if (renderersLoaded_) {
        return;
    }
    renderersLoaded_ = true;

#if defined(CHESS_HAS_QT_SVG)
    const PieceType types[] = {
        PieceType::Pawn, PieceType::Knight, PieceType::Bishop,
        PieceType::Rook, PieceType::Queen, PieceType::King
    };
    const Color colors[] = {Color::White, Color::Black};

    for (Color color : colors) {
        for (PieceType type : types) {
            const Piece piece{type, color};
            const std::string path = PieceAssetNames::resourcePath(piece);
            if (path.empty()) {
                continue;
            }
            const QString qpath = QString::fromStdString(path);
            QFile file(qpath);
            if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
                continue;
            }
            const QByteArray data = file.readAll();
            // Skip missing/placeholder assets so Unicode fallback is used.
            if (data.size() < 200 || data.contains("Placeholder")) {
                continue;
            }
            auto renderer = std::make_unique<QSvgRenderer>(data);
            if (renderer->isValid()) {
                renderers_.emplace(path, std::move(renderer));
            }
        }
    }
#endif
}

QChar ChessBoardWidget::unicodeGlyph(const Piece& piece) {
    // Unicode chess symbols (U+2654..U+265F).
    if (piece.isEmpty()) {
        return QChar();
    }
    static constexpr char16_t white[] = {0, u'♙', u'♘', u'♗', u'♖', u'♕', u'♔'};
    static constexpr char16_t black[] = {0, u'♟', u'♞', u'♝', u'♜', u'♛', u'♚'};
    const auto index = static_cast<int>(piece.type);
    if (index < 1 || index > 6) {
        return QChar();
    }
    return QChar(piece.color == Color::White ? white[index] : black[index]);
}

void ChessBoardWidget::drawPiece(QPainter& painter, const QRect& rect, const Piece& piece) const {
    if (piece.isEmpty()) {
        return;
    }

#if defined(CHESS_HAS_QT_SVG)
    const std::string path = PieceAssetNames::resourcePath(piece);
    const auto it = renderers_.find(path);
    if (it != renderers_.end() && it->second && it->second->isValid()) {
        // Inset slightly so pieces don't touch square borders.
        const QRect target = rect.adjusted(rect.width() / 10, rect.height() / 10,
                                           -rect.width() / 10, -rect.height() / 10);
        it->second->render(&painter, target);
        return;
    }
#endif

    // Graceful fallback when external SVGs are missing or empty placeholders.
    painter.save();
    QFont font = painter.font();
    font.setPixelSize(std::max(12, rect.height() * 3 / 4));
    painter.setFont(font);
    const QString glyph(unicodeGlyph(piece));
    painter.setPen(piece.color == Color::White ? QColor(30, 30, 30) : QColor(240, 240, 240));
    painter.drawText(rect, Qt::AlignCenter, glyph);
    painter.setPen(piece.color == Color::White ? QColor(250, 250, 250) : QColor(25, 25, 25));
    painter.drawText(rect.adjusted(1, 1, 1, 1), Qt::AlignCenter, glyph);
    painter.restore();
}

void ChessBoardWidget::paintEvent(QPaintEvent* /*event*/) {
    ensureRenderersLoaded();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRect area = boardRect();
    const int size = area.width();
    if (size <= 0) {
        return;
    }

    const QColor light(240, 217, 181);
    const QColor dark(181, 136, 99);
    const QColor selected(246, 246, 105, 180);
    const QColor legal(100, 180, 80, 140);

    for (int sq = 0; sq < 64; ++sq) {
        const auto mapped = BoardSquareMapping::squareRect(sq, size, whiteAtBottom_);
        const QRect rect(area.x() + mapped.x, area.y() + mapped.y, mapped.width, mapped.height);
        const bool isDark = ((BoardSquareMapping::fileOf(sq) + BoardSquareMapping::rankOf(sq)) % 2) != 0;
        painter.fillRect(rect, isDark ? dark : light);

        if (selection_.selectedSquare() == sq) {
            painter.fillRect(rect, selected);
        } else if (selection_.isLegalDestination(sq)) {
            painter.fillRect(rect, legal);
        }

        drawPiece(painter, rect, board_.pieceAt(sq));
    }

    painter.setPen(QPen(QColor(60, 40, 20), 2));
    painter.drawRect(area.adjusted(0, 0, -1, -1));
}

void ChessBoardWidget::mousePressEvent(QMouseEvent* event) {
    if (!inputEnabled_ || event->button() != Qt::LeftButton) {
        return;
    }

    const QRect area = boardRect();
    const QPoint local = event->pos() - area.topLeft();
    const Square square =
        BoardSquareMapping::squareAt(local.x(), local.y(), area.width(), whiteAtBottom_);
    if (square == kNoSquare) {
        return;
    }

    if (auto move = selection_.handleSquareClick(square)) {
        emit moveChosen(*move);
    }
    update();
}

void ChessBoardWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    update();
}

} // namespace chess
