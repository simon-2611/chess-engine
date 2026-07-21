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

UndoInfo Board::makeMove(const Move& move) {
    UndoInfo undo;
    undo.castlingRights = castlingRights_;
    undo.enPassantSquare = enPassantSquare_;
    undo.halfmoveClock = halfmoveClock_;
    undo.capturedPiece = move.capturedPiece();

    Piece p = squares_[move.from()];
    
    // Handle En Passant capture removal
    if (move.flag() == MoveFlag::EnPassantCapture) {
        int direction = (sideToMove_ == Color::White) ? 1 : -1;
        squares_[move.to() - direction * 8] = Piece{};
    }

    // Handle castling
    if (move.flag() == MoveFlag::CastleKingSide) {
        int rank = (sideToMove_ == Color::White) ? 0 : 7;
        squares_[rank * 8 + 5] = squares_[rank * 8 + 7]; // Move rook
        squares_[rank * 8 + 7] = Piece{};
    } else if (move.flag() == MoveFlag::CastleQueenSide) {
        int rank = (sideToMove_ == Color::White) ? 0 : 7;
        squares_[rank * 8 + 3] = squares_[rank * 8 + 0]; // Move rook
        squares_[rank * 8 + 0] = Piece{};
    }

    // Update board
    squares_[move.from()] = Piece{};
    if (move.isPromotion()) {
        PieceType promotedType;
        switch (move.flag()) {
            case MoveFlag::PromotionToQueen: promotedType = PieceType::Queen; break;
            case MoveFlag::PromotionToRook: promotedType = PieceType::Rook; break;
            case MoveFlag::PromotionToBishop: promotedType = PieceType::Bishop; break;
            case MoveFlag::PromotionToKnight: promotedType = PieceType::Knight; break;
            default: promotedType = PieceType::Queen; break;
        }
        squares_[move.to()] = Piece{promotedType, sideToMove_};
    } else {
        squares_[move.to()] = p;
    }

    // Update state
    if (sideToMove_ == Color::Black) {
        fullmoveNumber_++;
    }
    
    // Halfmove clock
    if (move.movedPiece() == PieceType::Pawn || move.isCapture()) {
        halfmoveClock_ = 0;
    } else {
        halfmoveClock_++;
    }

    // En Passant square
    if (move.flag() == MoveFlag::DoublePawnPush) {
        int direction = (sideToMove_ == Color::White) ? 1 : -1;
        enPassantSquare_ = move.from() + direction * 8;
    } else {
        enPassantSquare_ = kNoSquare;
    }

    // Castling rights
    if (move.movedPiece() == PieceType::King) {
        if (sideToMove_ == Color::White) {
            castlingRights_ &= ~(WhiteKingSide | WhiteQueenSide);
        } else {
            castlingRights_ &= ~(BlackKingSide | BlackQueenSide);
        }
    }
    // Rook moves or captures
    if (move.from() == 0 || move.to() == 0) castlingRights_ &= ~WhiteQueenSide;
    if (move.from() == 7 || move.to() == 7) castlingRights_ &= ~WhiteKingSide;
    if (move.from() == 56 || move.to() == 56) castlingRights_ &= ~BlackQueenSide;
    if (move.from() == 63 || move.to() == 63) castlingRights_ &= ~BlackKingSide;

    sideToMove_ = oppositeColor(sideToMove_);
    return undo;
}

void Board::undoMove(const Move& move, const UndoInfo& undo) {
    sideToMove_ = oppositeColor(sideToMove_);
    if (sideToMove_ == Color::Black) {
        fullmoveNumber_--;
    }

    castlingRights_ = undo.castlingRights;
    enPassantSquare_ = undo.enPassantSquare;
    halfmoveClock_ = undo.halfmoveClock;

    Piece p = squares_[move.to()];
    if (move.isPromotion()) {
        p = Piece{PieceType::Pawn, sideToMove_};
    }

    squares_[move.from()] = p;
    squares_[move.to()] = Piece{};

    if (undo.capturedPiece != PieceType::None) {
        if (move.flag() == MoveFlag::EnPassantCapture) {
            int direction = (sideToMove_ == Color::White) ? 1 : -1;
            squares_[move.to() - direction * 8] = Piece{PieceType::Pawn, oppositeColor(sideToMove_)};
        } else {
            squares_[move.to()] = Piece{undo.capturedPiece, oppositeColor(sideToMove_)};
        }
    }

    if (move.flag() == MoveFlag::CastleKingSide) {
        int rank = (sideToMove_ == Color::White) ? 0 : 7;
        squares_[rank * 8 + 7] = squares_[rank * 8 + 5];
        squares_[rank * 8 + 5] = Piece{};
    } else if (move.flag() == MoveFlag::CastleQueenSide) {
        int rank = (sideToMove_ == Color::White) ? 0 : 7;
        squares_[rank * 8 + 0] = squares_[rank * 8 + 3];
        squares_[rank * 8 + 3] = Piece{};
    }
}

bool Board::isSquareAttacked(Square square, Color byColor) const {
    // Check for attacks by knight
    static const std::vector<int> knightOffsets = {-17, -15, -10, -6, 6, 10, 15, 17};
    for (int offset : knightOffsets) {
        Square target = square + offset;
        if (target >= 0 && target < 64 && std::abs(target % 8 - square % 8) <= 2) {
            Piece p = pieceAt(target);
            if (p.type == PieceType::Knight && p.color == byColor) return true;
        }
    }

    // Check for attacks by king
    static const std::vector<int> kingOffsets = {-9, -8, -7, -1, 1, 7, 8, 9};
    for (int offset : kingOffsets) {
        Square target = square + offset;
        if (target >= 0 && target < 64 && std::abs(target % 8 - square % 8) <= 1) {
            Piece p = pieceAt(target);
            if (p.type == PieceType::King && p.color == byColor) return true;
        }
    }

    // Check for attacks by pawn
    int direction = (byColor == Color::White) ? -1 : 1;
    for (int dx : {-1, 1}) {
        int file = square % 8;
        if ((file == 0 && dx == -1) || (file == 7 && dx == 1)) continue;
        Square target = square + direction * 8 + dx;
        if (target >= 0 && target < 64) {
            Piece p = pieceAt(target);
            if (p.type == PieceType::Pawn && p.color == byColor) return true;
        }
    }

    // Sliding pieces
    static const std::vector<int> rookOffsets = {8, -8, 1, -1};
    static const std::vector<int> bishopOffsets = {9, 7, -9, -7};
    
    for (int offset : rookOffsets) {
        for (int i = 1; i < 8; ++i) {
            Square target = square + offset * i;
            if (target < 0 || target >= 64) break;
            int prevSquare = square + offset * (i - 1);
            if (std::abs(target % 8 - prevSquare % 8) > 1) break;
            Piece p = pieceAt(target);
            if (!p.isEmpty()) {
                if (p.color == byColor && (p.type == PieceType::Rook || p.type == PieceType::Queen)) return true;
                break;
            }
        }
    }
    for (int offset : bishopOffsets) {
        for (int i = 1; i < 8; ++i) {
            Square target = square + offset * i;
            if (target < 0 || target >= 64) break;
            int prevSquare = square + offset * (i - 1);
            if (std::abs(target % 8 - prevSquare % 8) > 1) break;
            Piece p = pieceAt(target);
            if (!p.isEmpty()) {
                if (p.color == byColor && (p.type == PieceType::Bishop || p.type == PieceType::Queen)) return true;
                break;
            }
        }
    }

    return false;
}

bool Board::isInCheck(Color color) const {
    Square kingPos = kNoSquare;
    for (int i = 0; i < 64; ++i) {
        if (squares_[i].type == PieceType::King && squares_[i].color == color) {
            kingPos = i;
            break;
        }
    }
    if (kingPos == kNoSquare) return false;
    return isSquareAttacked(kingPos, oppositeColor(color));
}

GameResult Board::evaluateGameResult(bool sideToMoveHasLegalMoves) const {
    return evaluateGameResult(sideToMoveHasLegalMoves, {});
}

GameResult Board::evaluateGameResult(bool sideToMoveHasLegalMoves, const std::vector<uint64_t>& positionHistory) const {
    if (halfmoveClock_ >= 100) {
        return GameResult::DrawByFiftyMoveRule;
    }

    // Repetition check: 3-fold repetition is a draw
    if (!positionHistory.empty()) {
        uint64_t currentHash = zobristHash();
        int count = 1;
        for (uint64_t h : positionHistory) {
            if (h == currentHash) {
                count++;
            }
        }
        if (count >= 3) {
            return GameResult::DrawByRepetition;
        }
    }

    if (!sideToMoveHasLegalMoves) {
        if (isInCheck(sideToMove_)) {
            return (sideToMove_ == Color::White) ? GameResult::BlackWinsByCheckmate : GameResult::WhiteWinsByCheckmate;
        }
        return GameResult::DrawByStalemate;
    }

    // Insufficient material detection
    int whitePieces = 0, blackPieces = 0;
    int whiteKnights = 0, blackKnights = 0;
    int whiteBishops = 0, blackBishops = 0;
    bool whiteBishopLight = false, whiteBishopDark = false;
    bool blackBishopLight = false, blackBishopDark = false;
    bool majorPieceOrPawn = false;

    for (int i = 0; i < 64; ++i) {
        Piece p = squares_[i];
        if (p.isEmpty() || p.type == PieceType::King) continue;

        if (p.type == PieceType::Pawn || p.type == PieceType::Rook || p.type == PieceType::Queen) {
            majorPieceOrPawn = true;
            break;
        }

        if (p.color == Color::White) {
            whitePieces++;
            if (p.type == PieceType::Knight) whiteKnights++;
            else if (p.type == PieceType::Bishop) {
                whiteBishops++;
                if (((i / 8) + (i % 8)) % 2 != 0) whiteBishopLight = true;
                else whiteBishopDark = true;
            }
        } else {
            blackPieces++;
            if (p.type == PieceType::Knight) blackKnights++;
            else if (p.type == PieceType::Bishop) {
                blackBishops++;
                if (((i / 8) + (i % 8)) % 2 != 0) blackBishopLight = true;
                else blackBishopDark = true;
            }
        }
    }

    if (!majorPieceOrPawn) {
        // K vs K
        if (whitePieces == 0 && blackPieces == 0) return GameResult::DrawByInsufficientMaterial;
        
        // KN vs K
        if ((whitePieces == 1 && whiteKnights == 1 && blackPieces == 0) ||
            (blackPieces == 1 && blackKnights == 1 && whitePieces == 0)) {
            return GameResult::DrawByInsufficientMaterial;
        }
        
        // KB vs K
        if ((whitePieces == 1 && whiteBishops == 1 && blackPieces == 0) ||
            (blackPieces == 1 && blackBishops == 1 && whitePieces == 0)) {
            return GameResult::DrawByInsufficientMaterial;
        }
        
        // KB vs KB (bishops on same color squares)
        if (whitePieces == 1 && whiteBishops == 1 && blackPieces == 1 && blackBishops == 1) {
            if ((whiteBishopLight && blackBishopLight) || (whiteBishopDark && blackBishopDark)) {
                return GameResult::DrawByInsufficientMaterial;
            }
        }
    }

    return GameResult::Ongoing;
}

uint64_t Board::zobristHash() const {
    uint64_t hash = 0;
    for (int i = 0; i < 64; ++i) {
        if (!squares_[i].isEmpty()) {
            // Very dummy hash for testing repetition logic
            hash ^= (static_cast<uint64_t>(squares_[i].type) << (i % 32));
            hash ^= (static_cast<uint64_t>(squares_[i].color) << ((i + 13) % 32));
        }
    }
    hash ^= (static_cast<uint64_t>(sideToMove_) << 40);
    return hash;
}

} // namespace chess
