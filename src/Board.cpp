#include "Board.h"
#include <sstream>
#include <vector>

namespace chess {

Board::Board() = default;

Board Board::startingPosition() {
    return fromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board Board::fromFEN(const std::string& fen) {
    Board board;
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(fen);
    while (std::getline(tokenStream, token, ' ')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 4) return board;

    // 1. Piece placement
    int rank = 7;
    int file = 0;
    for (char c : tokens[0]) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (std::isdigit(c)) {
            file += c - '0';
        } else {
            Piece piece;
            piece.color = std::isupper(c) ? Color::White : Color::Black;
            char lowC = std::tolower(c);
            if (lowC == 'p') piece.type = PieceType::Pawn;
            else if (lowC == 'n') piece.type = PieceType::Knight;
            else if (lowC == 'b') piece.type = PieceType::Bishop;
            else if (lowC == 'r') piece.type = PieceType::Rook;
            else if (lowC == 'q') piece.type = PieceType::Queen;
            else if (lowC == 'k') piece.type = PieceType::King;
            
            board.squares_[rank * 8 + file] = piece;
            file++;
        }
    }

    // 2. Side to move
    board.sideToMove_ = (tokens[1] == "w") ? Color::White : Color::Black;

    // 3. Castling rights
    board.castlingRights_ = CastlingRights::None;
    if (tokens[2] != "-") {
        for (char c : tokens[2]) {
            if (c == 'K') board.castlingRights_ |= CastlingRights::WhiteKingSide;
            else if (c == 'Q') board.castlingRights_ |= CastlingRights::WhiteQueenSide;
            else if (c == 'k') board.castlingRights_ |= CastlingRights::BlackKingSide;
            else if (c == 'q') board.castlingRights_ |= CastlingRights::BlackQueenSide;
        }
    }

    // 4. En passant square
    if (tokens[3] == "-") {
        board.enPassantSquare_ = kNoSquare;
    } else {
        int f = tokens[3][0] - 'a';
        int r = tokens[3][1] - '1';
        board.enPassantSquare_ = r * 8 + f;
    }

    // 5. Halfmove clock
    if (tokens.size() >= 5) {
        board.halfmoveClock_ = std::stoi(tokens[4]);
    }

    // 6. Fullmove number
    if (tokens.size() >= 6) {
        board.fullmoveNumber_ = std::stoi(tokens[5]);
    }

    return board;
}

std::string Board::toFEN() const {
    std::ostringstream fen;

    // 1. Piece placement
    for (int rank = 7; rank >= 0; rank--) {
        int emptyCount = 0;
        for (int file = 0; file < 8; file++) {
            Piece piece = squares_[rank * 8 + file];
            if (piece.isEmpty()) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen << emptyCount;
                    emptyCount = 0;
                }
                char c;
                switch (piece.type) {
                    case PieceType::Pawn:   c = 'p'; break;
                    case PieceType::Knight: c = 'n'; break;
                    case PieceType::Bishop: c = 'b'; break;
                    case PieceType::Rook:   c = 'r'; break;
                    case PieceType::Queen:  c = 'q'; break;
                    case PieceType::King:   c = 'k'; break;
                    default: c = '?'; break;
                }
                fen << (char)(piece.color == Color::White ? std::toupper(c) : c);
            }
        }
        if (emptyCount > 0) {
            fen << emptyCount;
        }
        if (rank > 0) {
            fen << '/';
        }
    }

    // 2. Side to move
    fen << " " << (sideToMove_ == Color::White ? "w" : "b");

    // 3. Castling rights
    fen << " ";
    if (castlingRights_ == CastlingRights::None) {
        fen << "-";
    } else {
        if (castlingRights_ & CastlingRights::WhiteKingSide)  fen << 'K';
        if (castlingRights_ & CastlingRights::WhiteQueenSide) fen << 'Q';
        if (castlingRights_ & CastlingRights::BlackKingSide)  fen << 'k';
        if (castlingRights_ & CastlingRights::BlackQueenSide) fen << 'q';
    }

    // 4. En passant square
    fen << " ";
    if (enPassantSquare_ == kNoSquare) {
        fen << "-";
    } else {
        char file = (char)('a' + (enPassantSquare_ % 8));
        char rank = (char)('1' + (enPassantSquare_ / 8));
        fen << file << rank;
    }

    // 5. Halfmove clock
    fen << " " << halfmoveClock_;

    // 6. Fullmove number
    fen << " " << fullmoveNumber_;

    return fen.str();
}

Piece Board::pieceAt(Square square) const {
    if (square < 0 || square >= 64) return Piece{};
    return squares_[square];
}

Color Board::sideToMove() const { return sideToMove_; }
uint8_t Board::castlingRights() const { return castlingRights_; }
Square Board::enPassantSquare() const { return enPassantSquare_; }
int Board::halfmoveClock() const { return halfmoveClock_; }
int Board::fullmoveNumber() const { return fullmoveNumber_; }

UndoInfo Board::makeMove(const Move& /*move*/) {
    // TODO: Move piece on squares_, handle special cases
    // (castling: move rook, en passant: remove captured pawn,
    // promotion: replace piece type), switch sideToMove_,
    // update castling rights/en passant square/halfmove clock.
    return UndoInfo{};
}

void Board::undoMove(const Move& /*move*/, const UndoInfo& /*undo*/) {
    // TODO: Fully undo makeMove(), including UndoInfo.
}

bool Board::isSquareAttacked(Square /*square*/, Color /*byColor*/) const {
    // TODO: Check if any opponent piece attacks 'square'.
    return false;
}

bool Board::isInCheck(Color /*color*/) const {
    // TODO: Determine king position, apply isSquareAttacked() on it.
    return false;
}

GameResult Board::evaluateGameResult(bool /*sideToMoveHasLegalMoves*/) const {
    // TODO: Check for checkmate/stalemate/50-move rule/repetition/insufficient material.
    return GameResult::Ongoing;
}

uint64_t Board::zobristHash() const {
    // TODO: Compose a hash from pre-generated random numbers per (square, piece, color)
    // as well as side to move/castling rights/en passant square.
    return 0;
}

} // namespace chess
