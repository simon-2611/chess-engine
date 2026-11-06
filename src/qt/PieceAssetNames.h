#pragma once

#include <string>

#include "Types.h"

namespace chess {

// Naming contract for optional external piece SVGs used by the Qt UI:
//   white_pawn.svg ... white_king.svg / black_pawn.svg ... black_king.svg
// Resource paths are exposed as ":/pieces/<name>.svg" via pieces.qrc.
struct PieceAssetNames {
    static std::string stem(const Piece& piece) {
        if (piece.isEmpty()) {
            return {};
        }
        std::string name = (piece.color == Color::White) ? "white_" : "black_";
        switch (piece.type) {
            case PieceType::Pawn:   name += "pawn"; break;
            case PieceType::Knight: name += "knight"; break;
            case PieceType::Bishop: name += "bishop"; break;
            case PieceType::Rook:   name += "rook"; break;
            case PieceType::Queen:  name += "queen"; break;
            case PieceType::King:   name += "king"; break;
            default: return {};
        }
        return name;
    }

    static std::string fileName(const Piece& piece) {
        const std::string s = stem(piece);
        if (s.empty()) {
            return {};
        }
        return s + ".svg";
    }

    static std::string resourcePath(const Piece& piece) {
        const std::string file = fileName(piece);
        if (file.empty()) {
            return {};
        }
        return ":/pieces/" + file;
    }
};

} // namespace chess
