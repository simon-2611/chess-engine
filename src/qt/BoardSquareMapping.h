#pragma once

#include <string>

#include "Types.h"

namespace chess {

// Pixel/geometry helpers for an 8x8 board widget. Pure logic (no Qt types) so
// click-to-square mapping can be unit-tested without a GUI.
struct BoardSquareMapping {
    struct Rect {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
    };

    static int fileOf(Square square) { return static_cast<int>(square) % 8; }
    static int rankOf(Square square) { return static_cast<int>(square) / 8; }

    static Square makeSquare(int file, int rank) {
        if (file < 0 || file > 7 || rank < 0 || rank > 7) {
            return kNoSquare;
        }
        return rank * 8 + file;
    }

    // Map a widget-local pixel to a square index. When whiteAtBottom is true,
    // rank 8 is drawn at the top of the widget (standard diagram orientation).
    static Square squareAt(int x, int y, int boardPixelSize, bool whiteAtBottom = true) {
        if (boardPixelSize <= 0 || x < 0 || y < 0 || x >= boardPixelSize || y >= boardPixelSize) {
            return kNoSquare;
        }
        const int squareSize = boardPixelSize / 8;
        if (squareSize <= 0) {
            return kNoSquare;
        }
        const int col = x / squareSize;
        const int row = y / squareSize;
        if (col < 0 || col > 7 || row < 0 || row > 7) {
            return kNoSquare;
        }
        const int file = whiteAtBottom ? col : (7 - col);
        const int rank = whiteAtBottom ? (7 - row) : row;
        return makeSquare(file, rank);
    }

    static Rect squareRect(Square square, int boardPixelSize, bool whiteAtBottom = true) {
        Rect rect;
        if (square < 0 || square > 63 || boardPixelSize <= 0) {
            return rect;
        }
        const int squareSize = boardPixelSize / 8;
        if (squareSize <= 0) {
            return rect;
        }
        const int file = fileOf(square);
        const int rank = rankOf(square);
        const int col = whiteAtBottom ? file : (7 - file);
        const int row = whiteAtBottom ? (7 - rank) : rank;
        rect.x = col * squareSize;
        rect.y = row * squareSize;
        rect.width = squareSize;
        rect.height = squareSize;
        return rect;
    }

    static std::string toAlgebraic(Square square) {
        if (square < 0 || square > 63) {
            return "";
        }
        std::string s;
        s.push_back(static_cast<char>('a' + fileOf(square)));
        s.push_back(static_cast<char>('1' + rankOf(square)));
        return s;
    }
};

} // namespace chess
